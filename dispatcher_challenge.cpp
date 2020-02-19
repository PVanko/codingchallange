#include <iostream>
#include <map>
#include <string>
#include <iostream>

//
// supporting tools and software
//
// Validate and test your json commands
// https://jsonlint.com/

// RapidJSON : lots and lots of examples to help you use it properly
// https://github.com/Tencent/rapidjson
//

// std::function
// std::bind
// std::placeholders
// std::map
// std::make_pair

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

bool g_done = false;

//
// TEST COMMANDS
//
auto help_command = R"(
 {
  "command":"help",
  "payload": {
    "usage":"Enter json command in 'command':'<command>','payload': { // json payload of arguments }",
  }
 }
)";

auto exit_command = R"(
 {
  "command":"exit",
  "payload": {
     "reason":"Exiting program on user request."
  }
 }
)";

auto sum_command = R"(
 {
  "command":"sum",
  "payload": {
    "numbers": [1, 2, 3, 4]
  }
 }
)";

auto author = R"(
 {
  "command":"salute",
  "payload": {
    "first_name":"Peter",
    "last_name":"Vanko"
  }
 }
)";

class Controller {
public:
    bool help(rapidjson::Value &payload)
    {
        cout << "Controller::help: command: ";
        if (payload.HasMember("usage")) {
            string strUsage = payload["usage"].GetString();
            cout << "Help instruction: Usage is" << endl << strUsage << endl << "Help end." << endl;
        }
        return true;
    }

    bool exit(rapidjson::Value &payload)
    {
        cout << "Controller::exit: command: \n";
        string strExitReason = "unspecified";
        if (payload.HasMember("reason")) {
            strExitReason = payload["reason"].GetString();
        }
        cout << "Program exit, reason: " << strExitReason << endl;
        g_done = true;
        return true;
    }

    bool sum(rapidjson::Value& payload)
    {   
        auto numbersItr = payload.FindMember("numbers");
        if (numbersItr == payload.MemberEnd()) {
            cout << "missing numbers\n";
            return false;
        }
        else if (!numbersItr->value.IsArray()) {
            cout << "numbers has to be array\n";
            return false;
        }

        auto& numbers = numbersItr->value;

        int sum = 0;
        for (auto& number : numbers.GetArray()) {
            sum += number.GetInt();
        }

        cout << "Sum of array is: " << sum << endl;

        return true;
    }

    bool author(rapidjson::Value& payload)
    {
        cout << "Controller::author: command: \n";
        string firstName;
        string lastName;
        if (payload.HasMember("first_name")) {
            firstName = payload["first_name"].GetString();
        }
        if (payload.HasMember("last_name")) {
            lastName = payload["last_name"].GetString();
        }

        cout << "Author: " << endl << firstName << " " <<lastName << endl;

        return true;
    }

};

// gimme ... this is actually tricky
// Bonus Question: why did I type cast this?
typedef std::function<bool(rapidjson::Value &)> CommandHandler;

// This was typecasted so we are declaring name for function

class CommandDispatcher {
public:
    // ctor - need impl
    CommandDispatcher()
    {
        // No implementation needed
    }

    // dtor - need impl
    virtual ~CommandDispatcher()
    {
        // question why is it virtual? Is it needed in this case?
        // It is virtual for future derivates of the class
    }

    bool addCommandHandler(std::string command, CommandHandler handler)
    {
        cout << "CommandDispatcher: addCommandHandler: " << command << std::endl;

        command_handlers_.insert(make_pair(command, handler));

        return true;
    }

    bool dispatchCommand(std::string command_json)
    {
        cout << "COMMAND: " << command_json << endl;

        Document doc;
        doc.Parse(command_json.c_str());
        rapidjson::Value& cmd = doc["command"];
        rapidjson::Value& payload = doc["payload"];
        CommandHandler valueFunc = command_handlers_.at(cmd.GetString());
        valueFunc(payload);
        std::cout << std::endl;
        
        return true;
    }

private:

    // gimme ...
    std::map<std::string, CommandHandler> command_handlers_;

    // another gimme ...
    // Question: why delete these?

    // To prevent copying of dispatcher

    // delete unused constructors
    CommandDispatcher (const CommandDispatcher&) = delete;
    CommandDispatcher& operator= (const CommandDispatcher&) = delete;

};

int main()
{
    std::cout << "COMMAND DISPATCHER: STARTED" << std::endl;

    CommandDispatcher command_dispatcher;
    Controller controller;                 // controller class of functions to "dispatch" from Command Dispatcher

    // Implement
    // add command handlers in Controller class to CommandDispatcher using addCommandHandler
    CommandHandler handle;
    //Initialize commands
    handle = std::bind(&Controller::exit, controller, std::placeholders::_1);
    addCommandHandler("exit", handle);
    handle = std::bind(&Controller::help, controller, std::placeholders::_1);
    addCommandHandler("help", handle);
    handle = std::bind(&Controller::sum, controller, std::placeholders::_1);
    addCommandHandler("version", handle);
    handle = std::bind(&Controller::author, controller, std::placeholders::_1);
    addCommandHandler("sum", handle);

    // gimme ...
    // command line interface
    string command;
    while( ! g_done ) {
        cout << "COMMANDS: {\"command\":\"exit\", \"payload\":{\"reason\":\"User requested exit.\"}}\n" 
            << "\t{\"command\":\"sum\", \"payload\": { [numbers]""}}\n"
            << "\t{\"command\":\"author\", \"payload\": { \"first name\": \"X\", \"last name\": \"X\" }}\n"
        cout << "\tenter command : ";
        getline(cin, command);
        command_dispatcher.dispatchCommand(command);
    }

    std::cout << "COMMAND DISPATCHER: ENDED" << std::endl;
    return 0;
}