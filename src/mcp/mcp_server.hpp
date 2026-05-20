#pragma once

/*
 * mcp_server.hpp — MCP (Model Context Protocol) server infrastructure
 *
 * Transport : stdio  (newline-delimited JSON-RPC 2.0)
 *
 * Dependencies:
 *   jsoncpp  (https://github.com/open-source-parsers/jsoncpp)
 *
 * Usage:
 *   #include "mcp_server.hpp"
 *
 *   MCPServer server;
 *   server.add_tool({ "name", "desc", {params...}, handler });
 *   server.run();
 */

#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <json/json.h>

#define MCP_SERVER_NAME         "butcher-mcp-server"
#define MCP_SERVER_VERSION      "1.0.0"

/* =========================================================================
 * Helpers — JSON serialisation / parsing
 * ========================================================================= */

inline std::string to_compact_string(const Json::Value& v) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "";
    builder["commentStyle"] = "None";
    return Json::writeString(builder, v);
}

inline Json::Value parse_line(const std::string& line) {
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::istringstream ss(line);
    if (!Json::parseFromStream(builder, ss, &root, &errs))
        throw std::runtime_error("JSON parse error: " + errs);
    return root;
}

/* =========================================================================
 * Tool infrastructure
 * ========================================================================= */

struct ToolParam {
    std::string name;
    std::string type;           // "string" | "number"
    std::string description;
    bool        required;
};

struct ToolDef {
    std::string name;
    std::string description;
    std::vector<ToolParam> params;
    std::function<std::string(const Json::Value&)> handler;
};

inline Json::Value schema_for(const std::vector<ToolParam>& params) {
    Json::Value schema(Json::objectValue);
    schema["type"]       = "object";
    schema["properties"] = Json::Value(Json::objectValue);
    schema["required"]   = Json::Value(Json::arrayValue);

    for (const auto& p : params) {
        Json::Value prop(Json::objectValue);
        prop["type"]        = p.type;
        prop["description"] = p.description;
        schema["properties"][p.name] = prop;
        if (p.required)
            schema["required"].append(p.name);
    }
    return schema;
}

/* =========================================================================
 * MCPServer
 * ========================================================================= */

class MCPServer {
    std::map<std::string, ToolDef> tools_;

    // ── JSON-RPC helpers ──────────────────────────────────────────────────

    Json::Value make_result(const Json::Value& id, Json::Value result) {
        Json::Value resp(Json::objectValue);
        resp["jsonrpc"] = "2.0";
        resp["id"]      = id;
        resp["result"]  = std::move(result);
        return resp;
    }

    Json::Value make_error(const Json::Value& id, int code, const std::string& msg) {
        Json::Value err(Json::objectValue);
        err["code"]    = code;
        err["message"] = msg;

        Json::Value resp(Json::objectValue);
        resp["jsonrpc"] = "2.0";
        resp["id"]      = id;
        resp["error"]   = std::move(err);
        return resp;
    }

    void send(const Json::Value& v) {
        std::cout << to_compact_string(v) << '\n';
        std::cout.flush();
    }

    // ── MCP method handlers ───────────────────────────────────────────────

    Json::Value handle_initialize(const Json::Value& /*params*/) {
        std::cerr << "[mcp] initialised\n";

        Json::Value serverInfo(Json::objectValue);
        serverInfo["name"]    = MCP_SERVER_NAME;
        serverInfo["version"] = MCP_SERVER_VERSION;

        Json::Value capabilities(Json::objectValue);
        capabilities["tools"] = Json::Value(Json::objectValue);

        Json::Value result(Json::objectValue);
        result["protocolVersion"] = "2024-11-05";
        result["serverInfo"]      = serverInfo;
        result["capabilities"]    = capabilities;
        return result;
    }

    Json::Value handle_tools_list(const Json::Value& /*params*/) {
        Json::Value list(Json::arrayValue);
        for (const auto& [name, def] : tools_) {
            Json::Value tool(Json::objectValue);
            tool["name"]        = def.name;
            tool["description"] = def.description;
            tool["inputSchema"] = schema_for(def.params);
            list.append(tool);
        }
        Json::Value result(Json::objectValue);
        result["tools"] = list;
        return result;
    }

    Json::Value handle_tools_call(const Json::Value& params) {
        if (!params.isMember("name") || !params["name"].isString())
            throw std::runtime_error("missing tool name");

        const std::string name = params["name"].asString();
        auto it = tools_.find(name);
        if (it == tools_.end())
            throw std::runtime_error("unknown tool: " + name);

        const Json::Value& args = params.isMember("arguments")
                                  ? params["arguments"]
                                  : Json::Value(Json::objectValue);

        const std::string text = it->second.handler(args);

        Json::Value content_item(Json::objectValue);
        content_item["type"] = "text";
        content_item["text"] = text;

        Json::Value content(Json::arrayValue);
        content.append(content_item);

        Json::Value result(Json::objectValue);
        result["content"] = content;
        result["isError"] = false;
        return result;
    }

    // ── dispatch ──────────────────────────────────────────────────────────

    void dispatch(const Json::Value& msg) {
        if (!msg.isMember("id")) {
            if (msg.isMember("method") &&
                msg["method"].asString() == "notifications/initialized")
                std::cerr << "[mcp] client confirmed initialised\n";
            return;
        }

        const Json::Value& id = msg["id"];

        if (!msg.isMember("method")) {
            send(make_error(id, -32600, "missing method"));
            return;
        }

        const std::string method = msg["method"].asString();
        std::cerr << "[mcp] -> " << method << '\n';

        const Json::Value& params = msg.isMember("params")
                                    ? msg["params"]
                                    : Json::Value(Json::objectValue);
        try {
            Json::Value result;
            if      (method == "initialize") result = handle_initialize(params);
            else if (method == "tools/list") result = handle_tools_list(params);
            else if (method == "tools/call") result = handle_tools_call(params);
            else {
                send(make_error(id, -32601, "method not found: " + method));
                return;
            }
            send(make_result(id, std::move(result)));
        } catch (const std::exception& e) {
            send(make_error(id, -32603, e.what()));
        }
    }

public:
    MCPServer& add_tool(ToolDef def) {
        tools_[def.name] = std::move(def);
        return *this;
    }

    void run() {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) continue;
            try {
                dispatch(parse_line(line));
            } catch (const std::exception& e) {
                std::cerr << "[mcp] error: " << e.what() << '\n';
            }
        }
    }
};

/*
#include "mcp_server.hpp"

#include <chrono>
#include <ctime>
#include <sstream>
*/

/* =========================================================================
 * Tool implementations
 * ========================================================================= */
/*
static std::string tool_echo(const Json::Value& args) {
    if (!args.isMember("text") || !args["text"].isString())
        return "(no text provided)";
    return "Echo: " + args["text"].asString();
}

static std::string tool_add(const Json::Value& args) {
    if (!args.isMember("a") || !args.isMember("b") ||
        !args["a"].isNumeric() || !args["b"].isNumeric())
        return "Error: a and b must be numbers";

    double a = args["a"].asDouble();
    double b = args["b"].asDouble();
    std::ostringstream os;
    os << a << " + " << b << " = " << (a + b);
    return os.str();
}

static std::string tool_get_time(const Json::Value& \/*args*\/) {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&t));
    return std::string("Current UTC time: ") + buf;
}
*/
/* =========================================================================
 * Entry point
 * ========================================================================= */
/*
int main() {
    std::cout.setf(std::ios::unitbuf);  // flush after every write

    MCPServer server;

    server
        .add_tool({
            "echo",
            "Returns whatever text you send it",
            {{"text", "string", "The text to echo back", true}},
            tool_echo
        })
        .add_tool({
            "add",
            "Sums two numbers",
            {
                {"a", "number", "First operand",  true},
                {"b", "number", "Second operand", true}
            },
            tool_add
        })
        .add_tool({
            "get_time",
            "Returns the current UTC time",
            {},
            tool_get_time
        });

    std::cerr << "[mcp] cpp-mcp-server ready (stdio)\n";
    server.run();
    return 0;
}
*/
