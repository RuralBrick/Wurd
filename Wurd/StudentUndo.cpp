#include "StudentUndo.h"

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
    if (!m_editorActions.empty()) {
        Entry& prevAction = m_editorActions.top();
        if (prevAction.action == Undo::Action::DELETE && prevAction.row == row) {
            if (prevAction.col == col) {
                prevAction.text.push_back(ch);
                return;
            }
            else if (prevAction.col == col + 1) {
                prevAction.col--;
                prevAction.text.insert(0, 1, ch);
                return;
            }
        }
        else if (prevAction.action == Undo::Action::INSERT && prevAction.row == row
                 && prevAction.col + prevAction.count == col) {
            prevAction.count++;
            return;
        }
    }
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
        col = undoAction.col - 1;
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
    while (!m_editorActions.empty())
        m_editorActions.pop();
}
