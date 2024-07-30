#include <iostream>
#include <cstdlib>
#include <iostream>
#include <time.h>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>
#include <vector>

class Maze
{
public:
    int **grid, sizeX, sizeY, state, posx = -1, posy = -1;
    int **init_grid;
    Maze(int sizex, int sizey)
    {
        sizeX = sizex;
        sizeY = sizey;
        grid = new int *[sizeX];
        for (int i = 0; i < sizeX; i++)
            grid[i] = new int[sizeY];

        init_grid = new int *[sizeX];
        for (int i = 0; i < sizeX; i++)
            init_grid[i] = new int[sizeY];

        for (int i = 0; i < sizeX; i++)
            for (int j = 0; j < sizeY; j++)
            {
                grid[i][j] = 0;
                init_grid[i][j] = 0;
            }
        
        state = -1;
    }

    void save()
    {
        for (int i = 0; i < sizeX; i++)
        {
            for (int j = 0; j < sizeY; j++)
            {
                init_grid[i][j] = grid[i][j];
            }
        }
    }

    void reset(int &_state)
    {
        
        for (int i = 0; i < sizeX; i++)
            for (int j = 0; j < sizeY; j++)
                grid[i][j] = init_grid[i][j];
        int xx, yy;
        int i = 0;
        do
        {
            xx = rand() % sizeX;
            yy = rand() % sizeY;
            i++;
        } while (grid[xx][yy] != 0 && i < sizeX * sizeY * 10);

        grid[xx][yy] = 7;
        state = xx * sizeY + yy;
        posx = xx;
        posy = yy;
        _state = state;
    }

    void move(int _action, bool &_isTerminal, int &_reward, int &_state)
    {
        grid[posx][posy] = 0;
        switch (_action)
        {
        case 0:
            posx = posx - 1;
            break;
        case 1:
            posx = posx + 1;
            break;
        case 2:
            posy = posy - 1;
            break;
        case 3:
            posy = posy + 1;
            break;
        default:
            exit(2);
            break;
        }
        if (posx < 0)
            posx = 0;
        if (posx >= sizeX)
            posx = sizeX - 1;
        if (posy < 0)
            posy = 0;
        if (posy >= sizeY)
            posy = sizeY - 1;
        if (grid[posx][posy] != 0)
        {
            _isTerminal = true;
            if (grid[posx][posy] == 8)
                _reward = 10 * sizeX * sizeY;
            if (grid[posx][posy] == 4)
                _reward = -10 * sizeX * sizeY;
        }
        else
        {
            _isTerminal = false;
            _reward = -1;
        }
        grid[posx][posy] = 7;
        state = posx * sizeY + posy;
        _state = state;
    }

    void render()
    {
        // system("clear");
        for (int i = 0; i < sizeX; i++)
        {
            for (int j = 0; j < sizeY; j++)
            {
                std::cout << grid[i][j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
    void set(int x, int y, int val)
    {
        grid[x][y] = val;
    }

    int get(int x, int y, int val)
    {
        return grid[x][y];
    }
};
int epsilon_greedy_policy(std::vector<std::vector<float>> &Q_table, int state, float epsilon)
{
    int action;
    float r = (rand() % 100) / 100.00f;
    if (r > epsilon)
    {
        float max = -9999;
        for (int i = 0; i < Q_table[0].size(); i++)
            if (Q_table[state][i] > max)
            {
                max = Q_table[state][i];
                action = i;
            }
    }
    else
    {
        action = rand() % 4;
    }
    return action;
}
int epsilon(std::vector<std::vector<float>> &Q_table, int state)
{
    int action;
    float max = -9999;
    for (int i = 0; i < Q_table[0].size(); i++)
        if (Q_table[state][i] > max)
        {
            max = Q_table[state][i];
            action = i;
        }
    return action;
}
void train(Maze &env, std::vector<std::vector<float>> &Q_table, float min_epsilon, float max_epsilon, float decay_rate, int max_steps, int n_training_episodes, float learning_rate, float gamma)
{
    int action, reward, state, new_state;
    bool terminal;
    float epsilon;
    for (int episode = 0; episode <= n_training_episodes; episode++)
    {
        epsilon = min_epsilon + (max_epsilon - min_epsilon) * exp(-decay_rate * episode);
        env.reset(state);
        // if (episode % 100 == 0)
        //     std::cout << episode << std::endl;

        for (int step = 0; step < max_steps; step++)
        {

            action = epsilon_greedy_policy(Q_table, state, epsilon);
            env.move(action, terminal, reward, new_state);
            float max = -9999;
            for (int i = 0; i < Q_table[0].size(); i++)
                if (Q_table[new_state][i] > max)
                {
                    max = Q_table[new_state][i];
                }
            Q_table[state][action] = Q_table[state][action] + learning_rate * (reward + gamma * max - Q_table[state][action]);
            state = new_state;
            if (terminal)
                break;
        }
    }
}

void test(std::vector<std::vector<float>> &Q_table, Maze Env)
{
    bool terminal = false;
    int state, reward;
    Env.reset(state);
    int action;
    while (!terminal)
    {
        action = epsilon(Q_table, state);
        Env.move(action, terminal, reward, state);
        Env.render();
    }
}







// int main()
// {
//     int sizex = 10, sizey = 10, actions = 4, n_training_episodes = 10000, n_eval_episodes = 100, max_steps = 5000;
//     float learning_rate = 0.7, gamma = 0.95, max_epsilon = 1, min_epsilon = 0.05, decay_rate = 0.0005;

//     std::vector<std::vector<float>> Q_table(sizex * sizey, std::vector<float>(actions, 0.0f));

//     for (int i = 0; i < sizex * sizey; i++)
//         for (int j = 0; j < actions; j++)
//             Q_table[i][j] = 0;

//     Maze Env(sizex, sizey);
//     Env.set(0, 8, 4);
//     Env.set(1, 8, 4);
//     Env.set(2, 8, 4);
//     Env.set(3, 8, 4);
//     Env.set(4, 8, 4);
//     Env.set(5, 8, 4);
//     Env.set(6, 8, 4);
//     Env.set(7, 8, 4);
//     Env.set(8, 8, 4);
//     Env.set(0, 9, 8);
//     Env.save();
//     int st;
//     Env.reset(st);
//     Env.render();

//     train(Env, Q_table, min_epsilon, max_epsilon, decay_rate, max_steps, n_training_episodes, learning_rate, gamma);

//     test(Q_table, Env);
// }