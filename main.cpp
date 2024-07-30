
#include "maze.cpp"
#include "buttons.cpp"
#define FRAME_LIMIT 3
enum STATE
{
    PAUSED,
    DRAW,
    TRAIN,
    PLAY
};

void init_map(std::vector<std::vector<sf::RectangleShape>> &GRID)
{

    for (int i = 0; i < GRID.size(); i++)
    {
        for (int j = 0; j < GRID[0].size(); j++)
        {
            GRID[i][j].setPosition(i * 50, j * 50);
            GRID[i][j].setSize(sf::Vector2f(45, 45));
        }
    }
}

void draw_map(sf::RenderWindow &window, std::vector<std::vector<sf::RectangleShape>> &GRID, int **grid)
{
    for (int i = 0; i < GRID.size(); i++)
    {
        for (int j = 0; j < GRID[0].size(); j++)
        {
            if (grid[i][j] == 0)
                GRID[i][j].setFillColor(sf::Color::White);
            if (grid[i][j] == 4)
                GRID[i][j].setFillColor(sf::Color::Green);
            if (grid[i][j] == 8)
                GRID[i][j].setFillColor(sf::Color::Yellow);
            if (grid[i][j] == 7)
                GRID[i][j].setFillColor(sf::Color::Blue);
            window.draw(GRID[i][j]);
        }
    }
}

void draw_buttons(sf::RenderWindow &window, std::vector<Button> &BUT)
{
    for (int i = 0; i < BUT.size(); i++)
        window.draw(BUT[i]);
}

bool get_pressed(std::vector<std::vector<sf::RectangleShape>> &GRID, sf::RenderWindow &window, int &x, int &y)
{

    auto mouse_pos = sf::Mouse::getPosition(window);          // Mouse position relative to the window
    auto translated_pos = window.mapPixelToCoords(mouse_pos); // Mouse position translated into world coordinates
    for (int i = 0; i < GRID.size(); i++)
        for (int j = 0; j < GRID[0].size(); j++)
            if (GRID[i][j].getGlobalBounds().contains(translated_pos))
            {
                x = i;
                y = j;
                return true;
            }
    return false;
}

int main()
{
   
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) // Make sure to have arial.ttf or use another font file
    {
        std::cerr << "Could not load font" << std::endl;
        return -1;
    }

    sf::Text T10;
    T10.setString("TRAIN\n10");
    T10.setFont(font);
    T10.setFillColor(sf::Color::White); 
    T10.setCharacterSize(22);
    sf::Text T100;
    T100.setString("TRAIN\n100");
    T100.setFont(font);
    T100.setFillColor(sf::Color::White); 
    T100.setCharacterSize(22);
    sf::Text T1k;
    T1k.setString("TRAIN\n1k");
    T1k.setFont(font);
    T1k.setFillColor(sf::Color::White); 
    T1k.setCharacterSize(22);
    sf::Text T10k;
    T10k.setString("TRAIN\n10k");
    T10k.setFont(font);
    T10k.setFillColor(sf::Color::White); 
    T10k.setCharacterSize(22);


    
    STATE GAME_STATE = PAUSED;
    int sizex = 20, sizey = 20, actions = 4, n_training_episodes = 10000, n_eval_episodes = 100, max_steps = 5000;
    float learning_rate = 0.7, gamma = 0.95, max_epsilon = 1, min_epsilon = 0.05, decay_rate = 0.0005;
    srand(time(0));
    std::vector<std::vector<float>> Q_table(sizex * sizey, std::vector<float>(actions, 0.0f));

    Maze Env(sizex, sizey);
    std::vector<std::vector<sf::RectangleShape>> GRID(sizex, std::vector<sf::RectangleShape>(sizey));
    for (int i = 0; i < sizex * sizey; i++)
        for (int j = 0; j < actions; j++)
            Q_table[i][j] = rand() % 100 - 50;

    std::vector<Button> BUT(8);
    for (int i = 0; i < BUT.size(); i++)
    {
        BUT[i].set(sizex, sizey, i);
    }
    sf::Texture texture_pause, texture_play, texture_reset, texture_draw, texture_poison;

    texture_play.loadFromFile("./textures/start.png");
    texture_pause.loadFromFile("./textures/pause.png");
    texture_reset.loadFromFile("./textures/reset.png");
    texture_draw.loadFromFile("textures/draw.png");
    texture_poison.loadFromFile("textures/poison.png");

    BUT[0].setTexture(&texture_play);
    BUT[1].setTexture(&texture_pause);
    BUT[2].setTexture(&texture_draw);
    BUT[3].setTexture(&texture_reset);

    T10.setPosition(BUT[4].getPosition());
    T10.move(sf::Vector2f(0,50));
    T100.setPosition(BUT[5].getPosition());
    T100.move(sf::Vector2f(0,50));
    T1k.setPosition(BUT[6].getPosition());
    T1k.move(sf::Vector2f(0,50));
    T10k.setPosition(BUT[7].getPosition());
    T10k.move(sf::Vector2f(0,50));

    Env.set(0, 9, 8);

    Env.save();

    init_map(GRID);

    //// se vor sterge
    bool terminal = false;
    int state, reward;
    int action;
    Env.reset(state);
    /// se vor sterge
    sf::RenderWindow window(sf::VideoMode(50 * sizex + 500, 50 * sizey), "puzzle");
    window.setFramerateLimit(FRAME_LIMIT);
    bool grid_pressed;
    int x_cood, y_coord;
    bool mouse_down = false;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                mouse_down = true;
                if (BUT[0].is_inside_me(window))
                {
                    window.setFramerateLimit(FRAME_LIMIT);
                    GAME_STATE = PLAY;
                    std::cout << "PLAY" << std::endl;
                }

                if (BUT[1].is_inside_me(window))
                {
                    GAME_STATE = PAUSED;
                    std::cout << "PAUSED" << std::endl;
                }
                if (BUT[2].is_inside_me(window))
                {
                    window.setFramerateLimit(60);
                    Env.set(Env.posx, Env.posy, 0);
                    GAME_STATE = DRAW;
                    std::cout << "DRAW" << std::endl;
                }
                if (BUT[3].is_inside_me(window))
                {
                    window.setFramerateLimit(FRAME_LIMIT);
                    std::cout << "RESET" << std::endl;
                    GAME_STATE = PLAY;
                    Env.reset(state);
                }

                if (BUT[4].is_inside_me(window))
                {
                    window.setFramerateLimit(FRAME_LIMIT);
                    n_training_episodes = 10;
                    GAME_STATE = TRAIN;
                    std::cout << "TRAIN10" << std::endl;
                }
                if (BUT[5].is_inside_me(window))
                {
                    window.setFramerateLimit(FRAME_LIMIT);
                    n_training_episodes = 100;
                    GAME_STATE = TRAIN;
                    std::cout << "TRAIN100" << std::endl;
                }
                if (BUT[6].is_inside_me(window))
                {
                    window.setFramerateLimit(FRAME_LIMIT);
                    n_training_episodes = 1000;
                    GAME_STATE = TRAIN;
                    std::cout << "TRAIN1k" << std::endl;
                }
                if (BUT[7].is_inside_me(window))
                {
                    window.setFramerateLimit(FRAME_LIMIT);
                    n_training_episodes = 10000;
                    GAME_STATE = TRAIN;
                    std::cout << "TRAIN10k" << std::endl;
                }
            }
            if (event.type == sf::Event::MouseButtonReleased)
            {
                mouse_down = false;
            }
        }

        switch (GAME_STATE)
        {
        case PAUSED:
            // do nothing
            break;
        case TRAIN:
            train(Env, Q_table, min_epsilon, max_epsilon, decay_rate, max_steps, n_training_episodes, learning_rate, gamma);
            GAME_STATE = PLAY;
            Env.reset(state);
        case PLAY:
            action = epsilon(Q_table, state);
            Env.move(action, terminal, reward, state);
            if (terminal == true)
                // Env.reset(state);
                GAME_STATE = PAUSED;
            break;
        case DRAW:

            // if (grid_pressed)
            // {
            if (mouse_down)
            {
                grid_pressed = get_pressed(GRID, window, x_cood, y_coord);
                if (grid_pressed)
                    Env.set(x_cood, y_coord, 4);
                Env.save();
            }

            // }
            break;
        default:
            break;
        }

        window.clear();
        draw_map(window, GRID, Env.grid);
        draw_buttons(window, BUT);
        window.draw(T10);
        window.draw(T100);
        window.draw(T1k);
        window.draw(T10k);
        window.display();
    }

    return 0;
}

// sf::String playerInput;
// sf::Text playerText;
// sf::Font font;
// if (!font.loadFromFile("arial.ttf")) // Make sure to have arial.ttf or use another font file
// {
//     std::cerr << "Could not load font" << std::endl;
//     return -1;
// }

// playerText.setFont(font);
// playerText.setFillColor(sf::Color::Red); // Use setFillColor instead of setColor
// playerText.setCharacterSize(24); // Set character size

// if (event.type == sf::Event::TextEntered)
// {
//     if(event.text.unicode == 8)
//     {
//         if(playerInput.getSize() > 0)
//             playerInput.erase(playerInput.getSize()-1,1);
//     }
//     else
//     if (event.text.unicode < 128) // Handle ASCII characters
//         playerInput += event.text.unicode;

//     playerText.setString(playerInput);
//     std::cout << playerInput.toAnsiString() << std::endl;
// }

// window.draw(playerText);