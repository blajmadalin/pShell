#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include <linux/limits.h> // for PATH_MAX
#include <unistd.h> // for chdir
#include <sstream>
#include <fstream>

std::string prev_dir;
namespace fs = std::filesystem;

void cmd_exit() { exit(0); }

void cmd_help(std::string arg) {
    std::unordered_map<std::string, std::string> commands = {
        {"help", "Help is here to help you learn how pShell works. \n"},
        {"cd", " "},
        {"pwd", "Outputs the current directory \n"},
        {"echo", "echo prints whatever the user has typed after space. \n"},
        {"ls", "ls lists all the files in the current directory. \n"},
        {"cat", "cat outputs the contents of a file. \n"},
        {"touch", "touch checks whether the file you tried to create exists and creates it if not. \n"},
        {"rm", "rm deletes the file the user has specified.\n"}
    };

    if(arg.empty()){
        std::cout<<"Available commands are: ";
        for(const auto& [commands, description] : commands){
            std::cout << commands << ", ";
        }
        std::cout << '\n';
        return;
    }
    if(commands.find(arg) != commands.end()){
        std::cout<<commands[arg];
    }
    

}

void cmd_cd(std::string &path){

    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return;
    }
    std::string current_dir = cwd;

    if(path.empty() || path[0] =='~'){ //if empty, go back
        const char* home = getenv("HOME");

        if(!home){
            std::cerr<<"cd: HOME environment variable not set\n";
            return;
        }
        path =std::string(home); 
        //std::cout<<path << '\n';   -- for testing
    }
    else if(path == "-" ){ //go to previous directory

        if(prev_dir.empty()){
            std::cerr<<"cd: no previous directory \n";
            return;
        }
        path = prev_dir;
    }
    else if(path == ".."){
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

void cmd_ls(){
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
            perror("getcwd");
            return;
        }
    std::string path = cwd;
    for(const auto entry : fs::directory_iterator(path)){
        std::cout<< entry.path().filename() << " ";
    }
    std::cout <<'\n';
    return;

}

void cmd_pwd(){
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    std::cout << cwd << std::endl;
    return;
}

void cmd_echo(std::string arg){
    std::cout<< arg << '\n';
}

void cmd_cat(std::string arg){
    std::ifstream file(arg);

    if(!file){
        std::cerr<<"cat: cannot open file \n";
    }
    std::string line;
    while(std::getline(file, line)){
        std::cout<<line<<'\n';
    }
}

void cmd_touch(std::string arg){ //function to create files or to check whether if it already exists
    std::ifstream file(arg); //getting the file with the same name, if it exists
    
    if(file.good()){
        std::cout << "File already exists \n"; //file.good() returns true if the file already exists
        return;
    }
    
    std::ofstream newFile(arg); //if the file doesen't exist, it gets created

}

void cmd_rm(std::string arg){ //function to remove files
    if(std::remove(arg.c_str()) == 0){ //if output of std::remove is 0, that means that the file was removed succesfully
        std::cout<<"File deleted succesfully \n";
        return;
    }
    std::cerr << "rm: file does not exist \n"; //if the output was not 0, it failed
}

int main() {
    std::unordered_map<std::string, std::function<void()>> no_args_command = {
        {"exit", cmd_exit},
        {"ls", cmd_ls},
        {"pwd", cmd_pwd}
    };

    std::unordered_map<std::string, std::function <void(std::string&)>> args_command ={
        {"cd", cmd_cd},
        {"echo", cmd_echo},
        {"cat", cmd_cat},
        {"touch", cmd_touch},
        {"rm", cmd_rm},
        {"help", cmd_help}
    };

    while (true) {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd)); //get current dir and store in cwd
       
        std::cout << "[" <<cwd<<"]$ ";
        std::string line;
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string command, arg;
        iss >> command;
        std:getline(iss >> std::ws, arg);

        if(no_args_command.find(command) != no_args_command.end()){
            if(!arg.empty()) std::cerr<<"This command does not take arguments \n";
            else no_args_command[command]();
        }

        else if(args_command.find(command) != args_command.end()){
            args_command[command](arg);
        }

        else{
            std::cerr<<"shell: Unknown command: " <<" '"<<command<<"'\n";
        } 
    }
    return 0;
}
