#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
    Entry newAction;
    newAction.action = action;
    newAction.row = row;
    newAction.col = col;
    newAction.count = 1;
    newAction.text = ch;
    m_editorActions.push(newAction);
}

StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) {
    return Action::ERROR;  // TODO
}

void StudentUndo::clear() {
	// TODO
}
