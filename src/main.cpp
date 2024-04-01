#include <CLI11.hpp>

#include "ConfigManager.h"
#include "GoogleEventsAPI.h"
#include "GoogleTasksAPI.h"

int main(int argc, char **argv) {
#ifdef _WIN32
    system("chcp 65001");  // Set the console to UTF-8 mode
#endif

    bool showCompleted = true;
    int daysBefore = 7;
    int daysAfter = 7;
    
    CLI::App app{"Google Calendar CLI"};
    argv = app.ensure_utf8(argv);

    app.require_subcommand(1);
    auto help = app.add_subcommand("help")->silent();
    help->parse_complete_callback([]() { throw CLI::CallForHelp(); });

    CLI::App *configApp =
        app.add_subcommand("config", "Configure the authentication settings");
    configApp->require_subcommand(1);
    auto configShow =
        configApp->add_subcommand("show", "Show the current configuration");
    configShow->callback([]() { ConfigManager::showConfiguration(); });

    auto configSet = configApp->add_subcommand("set", "Set the configuration");
    std::string credPath;
    configSet->add_option<std::string>("-f,--file", credPath,
                                       "Credential file path");
    configSet->callback(
        [&credPath]() { ConfigManager::setConfiguration(credPath); });

    auto *eventApp = app.add_subcommand("event", "Manage events");
    eventApp->require_subcommand(1);
    auto eventList = eventApp->add_subcommand("ls", "List events");
    eventList->add_option("-a,--days-after", daysAfter,
                          "Days after today to include in the event list");
    eventList->add_option("-b,--days-before", daysBefore,
                          "Days before today to include in the event list");

    eventList->callback([&]() {
        GoogleEventsAPI googleEventsAPI = GoogleEventsAPI();
        googleEventsAPI.list(daysBefore, daysAfter);
    });
    auto eventAdd = eventApp->add_subcommand("add", "Add an event");
    eventAdd->callback([]() {
        GoogleEventsAPI googleEventsAPI = GoogleEventsAPI();
        googleEventsAPI.add();
    });

    auto *taskApp = app.add_subcommand("task", "Manage tasks");
    taskApp->require_subcommand(1);
    auto taskList = taskApp->add_subcommand("ls", "List tasks");
    taskList->add_option("-a,--days-after", daysAfter,
                         "Days after today to include in the task list");
    taskList->add_option("-b,--days-before", daysBefore,
                         "Days before today to include in the task list");
    taskList->add_option<bool>(
        "-c,--show-completed", showCompleted,
        "Whether to show completed tasks. Default is true.");
    taskList->callback([&]() {
        try {
            GoogleTasksAPI googleTasksAPI = GoogleTasksAPI();
            googleTasksAPI.list(showCompleted, daysBefore, daysAfter);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    });

    auto taskAdd = taskApp->add_subcommand("add", "Create tasks");
    std::string title = "";
    std::string dueDate = "";
    taskAdd->add_option("-t,--title", title, "Title of the task");
    taskAdd->add_option(
        "-d,--date", dueDate,
        "Due date of the task. The format should be YYYY-MM-DD. The time "
        "portion of the timestamp is discarded when setting the due date.");
    taskAdd->callback([&]() {
        try {
            GoogleTasksAPI googleTasksAPI = GoogleTasksAPI();
            googleTasksAPI.add(title, dueDate);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    });

    auto taskEdit = taskApp->add_subcommand("edit", "Edit tasks");
    daysBefore = 3;
    daysAfter = 3;
    taskEdit->add_option("-a,--days-after", daysAfter,
                         "Days after today to include in the task list");
    taskEdit->add_option("-b,--days-before", daysBefore,
                         "Days before today to include in the task list");
    taskEdit->callback([&]() {
        try {
            GoogleTasksAPI googleTasksAPI = GoogleTasksAPI();
            googleTasksAPI.edit(daysBefore, daysAfter);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    });

    CLI11_PARSE(app, argc, argv);

    return 0;
}
