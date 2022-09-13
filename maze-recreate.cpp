#include"olcConsoleGameEngine.h"
#include<iostream>
#include<vector>
#include<stack>
using namespace std;
using namespace this_thread;
class mazeRecreate :public olcConsoleGameEngine {
public:
	mazeRecreate() {
		m_sAppName = L"Maze_Recreate";
	}
private:
	int m_mazeHeight, m_mazeWidth, m_pathWidth, m_visitedCells;
	int* m_maze;
	enum {
		CELL_PATH_N = 0x01,
		CELL_PATH_S = 0x02,
		CELL_PATH_E = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
	};
	stack<pair<int, int>>st;
protected:
	bool OnUserCreate()override {
		m_mazeHeight = 25;
		m_mazeWidth = 40;
		m_maze = new int[m_mazeHeight * m_mazeWidth];
		memset(m_maze, 0x00, m_mazeHeight * m_mazeWidth * sizeof(int));
		m_maze[0] = CELL_VISITED;
		m_pathWidth = 3;
		m_visitedCells = 1;
		st.push(make_pair(0, 0));
		return true;
	}
	bool OnUserUpdate(float fElapsedTime)override {
		//sleep_for(10ms);
		vector<int>neighbours;
		auto ck = [&](int x, int y)->int {
			return (st.top().second + y) * m_mazeWidth + (st.top().first + x);
		};
		if (m_visitedCells < m_mazeWidth * m_mazeHeight) {
			if (st.top().second > 0 && (m_maze[ck(0, -1)] & CELL_VISITED) == 0)neighbours.push_back(0);
			if (st.top().second < m_mazeHeight - 1 && (m_maze[ck(0, 1)] & CELL_VISITED) == 0)neighbours.push_back(1);
			if (st.top().first < m_mazeWidth - 1 && (m_maze[ck(1, 0)] & CELL_VISITED) == 0)neighbours.push_back(2);
			if (st.top().first > 0 && (m_maze[ck(-1, 0)] & CELL_VISITED) == 0)neighbours.push_back(3);
			if (!neighbours.empty()) {
				int next_dir = neighbours[rand() % neighbours.size()];
				switch (next_dir) {
				case 0:
					m_maze[ck(0, 0)] |= CELL_PATH_N;
					m_maze[ck(0, -1)] |= CELL_PATH_S;
					st.push(make_pair(st.top().first + 0, st.top().second - 1));
					break;
				case 2:
					m_maze[ck(0, 0)] |= CELL_PATH_E;
					m_maze[ck(1, 0)] |= CELL_PATH_W;
					st.push(make_pair(st.top().first + 1, st.top().second + 0));
					break;
				case 3:
					m_maze[ck(0, 0)] |= CELL_PATH_W;
					m_maze[ck(-1, 0)] |= CELL_PATH_E;
					st.push(make_pair(st.top().first - 1, st.top().second + 0));
					break;
				case 1:
					m_maze[ck(0, 0)] |= CELL_PATH_S;
					m_maze[ck(0, 1)] |= CELL_PATH_N;
					st.push(make_pair(st.top().first + 0, st.top().second + 1));
					break;
				}
				m_maze[ck(0, 0)] |= CELL_VISITED;
				m_visitedCells++;
			}
			else {
				st.pop();
			}
		}
		Fill(0, 0, m_nScreenWidth, m_nScreenHeight, L' ');
		for (int x = 0; x < m_mazeWidth; x++) {
			for (int y = 0; y < m_mazeHeight; y++) {
				for (int py = 0; py < m_pathWidth; py++) {
					for (int px = 0; px < m_pathWidth; px++) {
						if (m_maze[y * m_mazeWidth + x] & CELL_VISITED) Draw(x * (m_pathWidth + 1) + px, y * (m_pathWidth + 1) + py, PIXEL_SOLID, FG_WHITE);
						else Draw(x * (m_pathWidth + 1) + px, y * (m_pathWidth + 1) + py, PIXEL_SOLID, FG_BLUE);
					}
				}
				for (int p = 0; p < m_pathWidth; p++) {
					if (m_maze[y * m_mazeWidth + x] & CELL_PATH_S) Draw(x * (m_pathWidth + 1) + p, y * (m_pathWidth + 1) + m_pathWidth);
					if (m_maze[y * m_mazeWidth + x] & CELL_PATH_E) Draw(x * (m_pathWidth + 1) + m_pathWidth, y * (m_pathWidth + 1) + p);
				}
			}
		}
		return true;
	}
};
int main(void) {
	mazeRecreate mGame;
	if (mGame.ConstructConsole(160, 100, 8, 8))mGame.Start();
}