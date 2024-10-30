#include "headless.hpp"
int main()
{
    printf(WELCOME_TEXT);
    A_init_error_system();
    A_error_system->register_listener(EventType::OnError, [](Event &ev) { printf(static_cast<ErrorMessage &>(ev).get_message().c_str()); });
    palace *p;
    try
    {
        p = new palace; // does sanity checks
    }
    catch (std::runtime_error &e)
    {
        printf("We couldn't find your steam folder. Please input an alternate sourcemods path.\n");
    try_again:
        std::string path;
        getline(std::cin, path);
        if (!std::filesystem::exists(path))
        {
            printf("That's not a valid path. Probably. Try again. Note the folder needs to exist.\n");
            goto try_again;
        }
        p = new palace(path);
    }
    p->find_sourcemod_path();
    int code = p->init_games();
    switch (code)
    {
    case 0:
        printf("Initialization success!\n");
        break;
    case 1:
        printf("non-adastral game in folder\n");
        break;
    case 2:
        printf("Steam sourcemods dir was not found. Is steam installed?\n");
        return code;
    }
    p->fetch_server_data();
    for (const auto &i : p->server_games)
    {
        std::cout << "[headless] "
                  << "Game Available: " << i.second->name << " (" << i.first << ")" << std::endl;
    }
    // printf("[headless] Updating open_fortress.\n");
    p->install_game("open_fortress", "/tmp");  // you should check the force_verify to see if verification alone is needed.
    delete (p);
    printf("[headless] Everything done.\n");
    return 0;
}