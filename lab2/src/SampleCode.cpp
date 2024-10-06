
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <utility>
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int PIXEL_SIZE = 5;
const int GRID_WIDTH = WINDOW_WIDTH / PIXEL_SIZE;
const int GRID_HEIGHT = WINDOW_HEIGHT / PIXEL_SIZE;
void seedRandomGrid(std::vector<std::vector<bool>> &grid)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int x = 0; x < GRID_WIDTH; ++x)
    {
        for (int y = 0; y < GRID_HEIGHT; ++y)
        {
            grid[x][y] = (std::rand() % 2 == 0); // Randomly seed each pixel
        }
    }
}

int countNeighbors(const std::vector<std::vector<bool>> &grid, int x, int y)
{
    int count = 0;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int nx = (x + i + GRID_WIDTH) % GRID_WIDTH;
            int ny = (y + j + GRID_HEIGHT) % GRID_HEIGHT;
            count += grid[nx][ny];
        }
    }
    return count;
}
void updateGrid(std::vector<std::vector<bool>> &grid,
                std::vector<std::vector<bool>> &newGrid)
{
    for (int x = 0; x < GRID_WIDTH; ++x)
    {
        for (int y = 0; y < GRID_HEIGHT; ++y)
        {
            int neighbors = countNeighbors(grid, x, y);
            if (grid[x][y])
            {
                if (neighbors < 2 || neighbors > 3)
                {
                    newGrid[x][y] = false; // Cell dies
                }
            }
            else
            {
                if (neighbors == 3)
                {
                    newGrid[x][y] = true; // Cell becomes alive
                }
            }
        }
    }
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game of Life");
    window.setFramerateLimit(120); // Set frame rate to control speed
    std::vector<std::vector<bool>> grid_current(GRID_WIDTH,
                                                std::vector<bool>(GRID_HEIGHT, false));
    std::vector<std::vector<bool>> grid_next(GRID_WIDTH,
                                             std::vector<bool>(GRID_HEIGHT, false));
    std::vector<std::vector<bool>> &refCurrent = grid_current, &refNext = grid_next;
    seedRandomGrid(grid_current);
    unsigned long count = 0;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        if (count++ % 2)
        {
            refCurrent = grid_next;
            refNext = grid_current;
        }
        else
        {
            refCurrent = grid_current;
            refNext = grid_next;
        }
        updateGrid(grid_current, refNext);
        window.clear();
        for (int x = 0; x < GRID_WIDTH; ++x)
        {
            for (int y = 0; y < GRID_HEIGHT; ++y)
            {
                if (refNext[x][y])
                {
                    sf::RectangleShape cell(sf::Vector2f(PIXEL_SIZE, PIXEL_SIZE));
                    cell.setPosition(x * PIXEL_SIZE, y * PIXEL_SIZE);
                    cell.setFillColor(sf::Color::White);
                    window.draw(cell);
                }
            }
        }
        window.display();
    }
    return 0;
}
