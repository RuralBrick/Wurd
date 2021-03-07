#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <string>
#include <stack>

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct Entry {
		Action action;
		int row;
		int col;
		int count;
		std::string text;
	};
	std::stack<Entry> m_actions;
};

#endif // STUDENTUNDO_H_
