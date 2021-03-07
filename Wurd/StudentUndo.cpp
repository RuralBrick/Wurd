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
    if (m_editorActions.empty())
        return Undo::Action::ERROR;
    Entry undoAction = m_editorActions.top();
    m_editorActions.pop();
    switch (undoAction.action) {
    case Undo::Action::INSERT:
        break;
    case Undo::Action::DELETE:
        break;
    case Undo::Action::SPLIT:
        break;
    case Undo::Action::JOIN:
        break;
    }
}

void StudentUndo::clear() {
	// TODO
}
