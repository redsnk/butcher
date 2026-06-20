#include <stdio.h>
#include <unistd.h>
#include "src/arch/arch_elf.hpp"
#include "src/arch/arch_pe.hpp"
#include "src/lang/lang_c_x64.hpp"
#include "src/lang/lang_py_x64.hpp"
#include "src/base/base_x64.hpp"
#include "src/mcp/mcp_server.hpp"

#define MY_VERSION  "v0.0.19"

#define MAX_STR     (1024)

int opt_p = false;
int opt_t = false;
int opt_a = false;
int opt_m = false;
enum Languages opt_l = C;
std::set<uint64_t> include;
std::set<uint64_t> exclude;
std::set<uint64_t> incmem;
std::map<uint64_t, std::string> named;

uint64_t string_to_num(char *num) {
uint64_t n;

    if ((strlen(num) >= 2) && (num[0] == '0') && (num[1] == 'x')) {
        sscanf(num+2,"%llx",&n);
        return (n);
    }
    return (atol(num));
}

void parse_list(char *list,std::set<uint64_t> *s) {
char *t;
char  *p,*i;

    t = strdup(list);
    i = p = t;
    while (*i) {
        p = strchr(i,',');
        if (p != NULL) {
            *p = 0;
            s->insert(string_to_num(i));
            i = p+1;
        }
        else {
            s->insert(string_to_num(i));
            break;
        }
    }
    free(t);
}

#define MAX_BUFFER  (1024)

void parse_file(char *name,std::set<uint64_t> *s) {
FILE *f;
char buffer[MAX_BUFFER];
char *p;

    f = fopen(name,"r");
    if (f != NULL) {
        while (fgets(buffer,MAX_BUFFER-1,f) != NULL) {
            while ((p=strchr(buffer,'\n'))!= NULL) *p = 0;
            if (strlen(buffer)) {
                s->insert(string_to_num(buffer));
            }
        }
        fclose(f);
    }
    else {
        fprintf(stderr,"Error: parse_file not found '%s'.\n",name);
    }
}

void parse_param(char *param,std::set<uint64_t> *s) {
    if (param[0] == '@') {
        parse_file(param+1,s);
    }
    else {
        parse_list(param,s);
    }
}

void parse_named(char *name) {
FILE *f;
char buffer[MAX_BUFFER];
char *p;

    f = fopen(name,"r");
    if (f != NULL) {
        while (fgets(buffer,MAX_BUFFER-1,f) != NULL) {
            while ((p=strchr(buffer,'\n'))!= NULL) *p = 0;
            if (strlen(buffer)) {
                p = strchr(buffer,',');
                if (p != NULL) {
                    *p = 0;
                    //printf("named: '%s'\n",p+1);
                    named.insert({ string_to_num(buffer),p+1 });
                }
                else {
                    fprintf(stderr,"Error: named format.\n");
                }
            }
        }
        fclose(f);
    }
}

//Butcher *butcher(char *path,uint64_t addr) {
Butcher *butcher(char *path) {
Butcher *b;
Language *l;
Archive *a;


    a = new Arch_Pe();
    if (!a->CheckFile(path)) {
        delete a;
        a = new Arch_Elf();
        if (!a->CheckFile(path)) {
            delete a;
            fprintf(stderr,"Error: format not compatible: '%s'\n",path);
            return (NULL);
        }
    }
    switch (opt_l) {
        case PYTHON:
            l = (Language *) new Lang_Py_x64(a->Is32()?32:64);
            break;
        case C:
        default:
            l = (Language *) new Lang_C_x64();
            break;
    }
    b = new Base_x64(a,l);
    b->ltraces = opt_t;
    b->lasm = opt_a;
    b->loadm = opt_m;
    b->in = include;
    b->ex = exclude;
    b->mi = incmem;
    b->named = named;
    /*
    b->Cut(path,addr);
    delete b;
    return (1);
    */
    return(b);
}

static std::string tool_cut(const Json::Value& args) {
char *out = NULL;
std::string vout;

    if (!args.isMember("path") || !args["path"].isString())
        return "(no path provided)";
    char *path = strdup((char *) args["path"].asString().c_str());
    //
    if (!args.isMember("addr") || !args["addr"].isString())
        return "(no address provided)";
    uint64_t addr = string_to_num((char *) args["addr"].asString().c_str());
    //
    if (args.isMember("l") && args["l"].isString()) {
        switch (args["l"].asString().c_str()[0]) {
            case 'c':
                opt_l = C;
                break;
            case 'p':
                opt_l = PYTHON;
                break;
            default:
                return "(output language unknown)";
        }
    }
    //
    if (args.isMember("m") && args["m"].isBool()) {
        opt_m = args["m"].asBool();
    }
    //
    if (args.isMember("t") && args["t"].isBool()) {
        opt_t = args["t"].asBool();
    }
    //
    if (args.isMember("a") && args["a"].isBool()) {
        opt_m = args["a"].asBool();
    }
    //
    if (args.isMember("i") && args["i"].isString()) {
        parse_param((char *)args["i"].asString().c_str(),&include);
    }
    //
    if (args.isMember("e") && args["e"].isString()) {
        parse_param((char *)args["e"].asString().c_str(),&exclude);
    }
    //
    if (args.isMember("u") && args["u"].isString()) {
        parse_param((char *)args["u"].asString().c_str(),&incmem);
    }
    //
    if (args.isMember("n") && args["n"].isString()) {
        parse_named((char *)args["n"].asString().c_str());
    }
    //
    Butcher *b = butcher(path);
    //printf("%s\n",path);
    if (b != NULL) {
        out = b->Cut(path,addr,false);
        delete b;
    }
    if (out != NULL) {
        vout = out;
        free(out);
    }
    else {
        vout = "Error.";
    }
    free(path);
    return (vout);
}

int butcher_mcp(void) {
    std::cout.setf(std::ios::unitbuf);
    MCPServer server;
    server.add_tool({
            "cut",
            "Decompiles a file from an address",
            {
                {"path", "string", "Path of the file", true},
                {"addr", "string", "Address of the function to decompile",  true},
                {"l", "string", "Output language, 'c' or 'p'",  false},
                {"m", "bool", "Load ALL memory from the original file at start",  false},
                {"t", "bool", "Include commented traces",  false},
                {"a", "bool", "Include commented asm code",  false},
                {"i", "string", "Include call addresses: list 'addr1,addr2,..' or file '@file.txt'",  false},
                {"e", "string", "Exclude call addresses: list 'addr1,addr2,..' or file '@file.txt'",  false},
                {"u", "string", "Include memory addresses: list 'addr1,addr2,..' or file '@file.txt'",  false},
                {"n", "string", "Add named functions from file: 'file.txt'",  false}
            },
            tool_cut
        });
    std::cerr << "[mcp] " MCP_SERVER_NAME "(" MCP_SERVER_VERSION ") ready (stdio)\n";
    server.run();
    return (1);
}

const char* HELP = "\
--------------------------------------------------------------\n\
Butcher (" MY_VERSION ") programed by Alex Bassas.\n\
--------------------------------------------------------------\n\
usage: Butcher [-p][-l<lang>][-m][-a][-t][-e<addr,addr,...>][-n<file>] <path> <addr>\n\
\n\
-l<lang>    => Output language: [c|p]\n\
\
                            c -> C\n\
                            p -> Python\n\
\n\
-p                          => Stdin MCP server\n\
-m                          => Load ALL memory from the original file at start\n\
-t                          => Include commented traces\n\
-a                          => Include commented asm code\n\
-i[addr,addr,...]|@file     => Include call addresses\n\
-e[addr,addr,...]|@file     => Exclude call addresses\n\
-u[addr,addr,...]|@file     => Include memory addresses\n\
-n[file]                    => Add named functions from <file>\n\
\n";

int main (int argc, char **argv) {
int p,i;
char path[MAX_STR];
uint64_t addr;

    while ((p = getopt(argc,argv,"pmtal:i:e:n:u:")) != -1) {
        switch (p) {
                case 'p':
                    opt_p = true;
                    break;
                case 't':
                    opt_t = true;
                    break;
                case 'a':
                    opt_a = true;
                    break;
                case 'm':
                    opt_m = true;
                    break;
                case 'l':
                    switch (optarg[0]) {
                        case 'c':
                            opt_l = C;
                            break;
                        case 'p':
                            opt_l = PYTHON;
                            break;
                    }
                    break;
                case 'i':
                    parse_param(optarg,&include);
                    break;
                case 'e':
                    parse_param(optarg,&exclude);
                    break;
                case 'u':
                    parse_param(optarg,&incmem);
                    break;
                case 'n':
                    parse_named(optarg);
                    break;
                case '?':
                default:
                        fprintf(stderr,HELP);
                        exit(0);
        }
    }
    path[0] = 0;
    if (optind == argc) {
        if (opt_p) {
            return(butcher_mcp());
        }
        else {
            fprintf(stderr,HELP);
            exit(0);
        }
    }
    if ((optind+1)<argc) {
        strcpy(path,argv[optind]);
        addr = string_to_num(argv[optind+1]);
    }
    else {
        fprintf(stderr,HELP);
        exit(0);
    }
    Butcher *b = butcher(path);
    if (b != NULL) {
        b->Cut(path,addr,true);
        delete b;
    }
    return(1);
}
