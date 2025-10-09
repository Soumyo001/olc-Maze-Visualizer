#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

class MazeRecreate : public olc::PixelGameEngine {
public:
    MazeRecreate() { sAppName = "Maze_Recreate"; }

private:
    int m_mazeHeight, m_mazeWidth, m_pathWidth, m_visitedCells;
    std::vector<int> m_maze;
    std::stack<std::pair<int, int>> st;

    enum {
        CELL_PATH_N = 0x01,
        CELL_PATH_S = 0x02,
        CELL_PATH_E = 0x04,
        CELL_PATH_W = 0x08,
        CELL_VISITED = 0x10,
    };

	void ResetMaze() {
    	m_maze.assign(m_mazeHeight * m_mazeWidth, 0);
    	m_maze[0] = CELL_VISITED;
    	m_visitedCells = 1;

    	while (!st.empty()) st.pop();
    	st.push({0, 0});
	}

public:
	bool OnUserCreate() override {
    	srand(time(nullptr));

    	m_mazeWidth  = 40;
    	m_mazeHeight = 25;

    	// compute path width
    	float cellPlusWallX = (float)ScreenWidth()  / (float)m_mazeWidth;
    	float cellPlusWallY = (float)ScreenHeight() / (float)m_mazeHeight;
    	m_pathWidth = std::min(cellPlusWallX, cellPlusWallY) - 1;

    	// adjust to make last cell align with bottom edge
    	float usedHeight = m_mazeHeight * (m_pathWidth + 1);
    	float scale = (float)ScreenHeight() / usedHeight;
    	m_pathWidth *= scale;

    	ResetMaze();

    	return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

	    if (GetKey(olc::ESCAPE).bReleased) return false;
		//	Regenarate maze
    	if (GetKey(olc::R).bPressed) {
    	    srand(time(nullptr));
    	    ResetMaze();
    	}
	
	    std::vector<int> neighbours;
	    auto idx = [&](int x, int y) {
	        return (st.top().second + y) * m_mazeWidth + (st.top().first + x);
	    };
	
	    if (m_visitedCells < m_mazeWidth * m_mazeHeight) {
	        if (st.top().second > 0 && !(m_maze[idx(0, -1)] & CELL_VISITED)) neighbours.push_back(0);
	        if (st.top().second < m_mazeHeight - 1 && !(m_maze[idx(0, 1)] & CELL_VISITED)) neighbours.push_back(1);
	        if (st.top().first < m_mazeWidth - 1 && !(m_maze[idx(1, 0)] & CELL_VISITED)) neighbours.push_back(2);
	        if (st.top().first > 0 && !(m_maze[idx(-1, 0)] & CELL_VISITED)) neighbours.push_back(3);
		
	        if (!neighbours.empty()) {
	            int next_dir = neighbours[rand() % neighbours.size()];
	            switch (next_dir) {
	                case 0: m_maze[idx(0, 0)] |= CELL_PATH_N; m_maze[idx(0, -1)] |= CELL_PATH_S; st.push({st.top().first, st.top().second - 1}); break;
	                case 1: m_maze[idx(0, 0)] |= CELL_PATH_S; m_maze[idx(0, 1)] |= CELL_PATH_N; st.push({st.top().first, st.top().second + 1}); break;
	                case 2: m_maze[idx(0, 0)] |= CELL_PATH_E; m_maze[idx(1, 0)] |= CELL_PATH_W; st.push({st.top().first + 1, st.top().second}); break;
	                case 3: m_maze[idx(0, 0)] |= CELL_PATH_W; m_maze[idx(-1, 0)] |= CELL_PATH_E; st.push({st.top().first - 1, st.top().second}); break;
	            }
	            m_maze[idx(0, 0)] |= CELL_VISITED;
	            m_visitedCells++;
	        } else {
	            st.pop();
	        }
	    }
	
	    Clear(olc::VERY_DARK_GREEN);
	
	    for (int y = 0; y < m_mazeHeight; y++) {
	        for (int x = 0; x < m_mazeWidth; x++) {
	            olc::Pixel col = (m_maze[y * m_mazeWidth + x] & CELL_VISITED)
	                             ? olc::WHITE : olc::GREEN;
	            FillRect(x * (m_pathWidth + 1),
	                     y * (m_pathWidth + 1),
	                     m_pathWidth, m_pathWidth, col);
			
	            if (m_maze[y * m_mazeWidth + x] & CELL_PATH_S)
	                FillRect(x * (m_pathWidth + 1),
	                         y * (m_pathWidth + 1) + m_pathWidth,
	                         m_pathWidth, 1, col);
			
	            if (m_maze[y * m_mazeWidth + x] & CELL_PATH_E)
	                FillRect(x * (m_pathWidth + 1) + m_pathWidth,
	                         y * (m_pathWidth + 1),
	                         1, m_pathWidth, col);
	        }
	    }
		
		DrawString(4, 4, "Press R to regenerate & ESC to quit", olc::BLUE);
	    return true;
	}

};

int main() {
    MazeRecreate demo;
    if (demo.Construct(640, 480, 2, 2))
        demo.Start();
    return 0;
}
