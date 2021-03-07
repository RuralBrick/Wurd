#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
    // TODO: Implement batching feature
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
        row = undoAction.row;
        col = undoAction.col;
        count = undoAction.count;
        text = "";
        return Undo::Action::DELETE;
    case Undo::Action::DELETE:
        row = undoAction.row;
        col = undoAction.col;
        count = 1;
        text = undoAction.text;
        return Undo::Action::INSERT;
    case Undo::Action::SPLIT:
        row = undoAction.row;
        col = undoAction.col;
        count = 1;
        text = "";
        return Undo::Action::JOIN;
    case Undo::Action::JOIN:
        row = undoAction.row;
        col = undoAction.col;
        count = 1;
        text = "";
        return Undo::Action::SPLIT;
    }
    return Undo::Action::ERROR;
}

void StudentUndo::clear() {
	// TODO
}
