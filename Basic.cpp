#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include <linux/limits.h> // for PATH_MAX
#include <unistd.h> // for chdir
#include <sstream>

void cmd_exit() { exit(0); }

void cmd_help() { std::cout << "Available commands are: help, cd, exit\n"; }

std::string prev_dir;
void cmd_cd(std::string &path){

    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return;
    }
    std::string current_dir = cwd;

    if(path.empty() || path[0] =='~'){ //if empty, go back
        const char* home = getenv("HOME");
//hello worlddd

        if(!home){
            std::cerr<<"cd: HOME environment variable not set\n";
            return;
        }
        path =std::string(home); 
        //std::cout<<path << '\n';
    }
    else if(path[0] == '-' ){ //go to previous directory

        if(prev_dir.empty()){
            std::cerr<<"cd: no previous directory";
            return;
        }
        path = prev_dir;
    }
    else if(path[0] == '.' && path[1] == '.'){
        chdir("..");
        return;
    }
    else if(path[0] != '/'){
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            path = std::string(cwd) + "/" + path;
        } 
        else {
            perror("getcwd");
            return;
        }
    }

    if (chdir(path.c_str()) != 0) {
        perror("cd failed");
        return;
    }

    prev_dir = current_dir;
}

int main() {
    std::unordered_map<std::string, std::function<void()>> no_args_command = {
        {"exit", cmd_exit},
        {"help", cmd_help}
    };

    std::unordered_map<std::string, std::function <void(std::string&)>> args_command ={
        {"cd", cmd_cd}
    };

    while (true) {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd)); //get current dir and store in cwd
       
        std::cout << "[" <<cwd<<"]$ ";
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string command, arg;
        iss >> command >> arg;


        if(no_args_command.find(command) != no_args_command.end()){
            if(!arg.empty()) std::cerr<<"This command does not take arguments \n";
            else no_args_command[command]();
        }
        else if(args_command.find(command) != args_command.end()){
            args_command[command](arg);
        }
        else{
            std::cerr<<"shell: Unknown command f" <<" '"<<command<<"'\n";

        } 
    }
    return 0;
}
