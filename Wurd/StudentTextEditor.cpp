#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo), m_row(0), m_col(0), m_numLines(1) {
	m_lines.push_back("");
	m_curLine = m_lines.begin();
}

StudentTextEditor::~StudentTextEditor() {
	// ~list<string> O(N)
}

void StudentTextEditor::resetCursor() {
	m_curLine = m_lines.begin();
	m_row = 0;
	m_col = 0;
}

bool StudentTextEditor::load(std::string file) {
	ifstream infile(file);
	if (!infile)
		return false;
	reset(); // O(M)
	string line;
	while (getline(infile, line)) { // O(N)
		if (line.back() == '\r')
			line.pop_back();
		m_lines.push_back(line);
		m_numLines++;
	}
	resetCursor(); // O(1)
	return true;
}

bool StudentTextEditor::save(std::string file) {
	ofstream outfile(file);
	if (!outfile)
		return false;
	for (auto it = m_lines.begin(); it != m_lines.end(); ++it) // O(M)
		outfile << *it << '\n';
	return true;
}

void StudentTextEditor::reset() {
	m_lines.clear(); // O(N)
	resetCursor(); // O(1)
	m_numLines = 0;
	getUndo()->clear(); // O(N)
}

void StudentTextEditor::insertAtCursor(char ch) {
	m_curLine->insert(m_col, 1, ch); // O(L)
	m_col++;
}

void StudentTextEditor::insert(char ch) {
	if (ch == '\t') {
		for (int i = 0; i < 4; ++i) { // O(4L)
			insertAtCursor(' ');
			getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ' ');
		}
	}
	else {
		insertAtCursor(ch); // O(L)
		getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch); // O(1)
	}
}

void StudentTextEditor::splitAtCursor() {
	auto curLineItr = m_curLine;
	string newLine = curLineItr->substr(m_col); // O(L)
	curLineItr->erase(m_col); // O(L)
	m_curLine = m_lines.insert(++curLineItr, newLine); // O(1)
}

void StudentTextEditor::enter() {
	splitAtCursor(); // O(L)
	getUndo()->submit(Undo::Action::SPLIT, m_row, m_col); // O(1)
	m_row++;
	m_col = 0;
	m_numLines++;
}

char StudentTextEditor::eraseAtCursor() {
	char deletedChar = m_curLine->at(m_col);
	m_curLine->erase(m_col, 1); // O(L)
	return deletedChar;
}

void StudentTextEditor::joinAtCursor() {
	auto nextLineItr = m_curLine;
	string nextLine = *(++nextLineItr); // O(L2)
	m_curLine->append(nextLine); // O(L1 + L2)
	m_lines.erase(nextLineItr); // O(1)
	m_numLines--;
}

void StudentTextEditor::del() {
	if (m_col < m_curLine->size()) {
		char deletedChar = eraseAtCursor(); // O(L)
		getUndo()->submit(Undo::Action::DELETE, m_row, m_col, deletedChar); // O(1)
	}
	else if (m_row < m_numLines - 1) {
		joinAtCursor(); // O(L1 + L2)
		getUndo()->submit(Undo::Action::JOIN, m_row, m_col); // O(1)
	}
}

void StudentTextEditor::backspace() {
	if (m_col > 0) {
		m_col--;
		char deletedChar = eraseAtCursor(); // O(L)
		getUndo()->submit(Undo::Action::DELETE, m_row, m_col, deletedChar); // O(1)
	}
	else if (m_row > 0) {
		m_curLine--;
		m_row--;
		m_col = m_curLine->size(); // O(1)
		joinAtCursor(); // O(L1 + L2)
		getUndo()->submit(Undo::Action::JOIN, m_row, m_col); // O(1)
	}
}

void StudentTextEditor::move(Dir dir) {
	switch (dir) {
	case Dir::UP:
		if (m_row > 0) {
			m_curLine--;
			m_row--;
			if (m_col > m_curLine->size())
				m_col = m_curLine->size();
		}
		break;
	case Dir::DOWN:
		if (m_row < m_numLines - 1) {
			m_curLine++;
			m_row++;
			if (m_col > m_curLine->size())
				m_col = m_curLine->size();
		}
		break;
	case Dir::LEFT:
		if (m_col > 0)
			m_col--;
		else if (m_row > 0) {
			m_curLine--;
			m_row--;
			m_col = m_curLine->size();
		}
		break;
	case Dir::RIGHT:
		if (m_col < m_curLine->size())
			m_col++;
		else if (m_row < m_numLines - 1) {
			m_curLine++;
			m_row++;
			m_col = 0;
		}
		break;
	case Dir::HOME:
		m_col = 0;
		break;
	case Dir::END:
		m_col = m_curLine->size();
		break;
	}
}

void StudentTextEditor::getPos(int& row, int& col) const {
	row = m_row;
	col = m_col;
}

void StudentTextEditor::moveToRow(list<string>::iterator& it, int& row, int targetRow) const {
	if (row > targetRow) {
		while (row > targetRow) { // O(abs(row - targetRow))
			it--;
			row--;
		}
	}
	else {
		while (row < targetRow) {
			it++;
			row++;
		}
	}
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	if (startRow < 0 || numRows < 0 || startRow > m_numLines)
		return -1;

	// Create and move iterator to startRow
	auto startRowItr = m_curLine;
	int curRow = m_row;
	moveToRow(startRowItr, curRow, startRow); // O(abs(curRow - startRow))

	// Empty &lines
	lines.clear(); // O(oldR)

	// Copy lines to &lines
	int linesCopied = 0;
	while (startRowItr != m_lines.end() && curRow < m_numLines && linesCopied < numRows) { // O(numRows * L)
		lines.push_back(*startRowItr); // O(L)
		startRowItr++;
		curRow++;
		linesCopied++;
	}

	return linesCopied;
}

void StudentTextEditor::moveCursor(int row, int col) {
	moveToRow(m_curLine, m_row, row); // O(abs(m_row - row))
	if (col > m_curLine->size())
		m_col = m_curLine->size();
	else
		m_col = col;
}

void StudentTextEditor::undo() {
	int row = m_row;
	int col = m_col;
	int count = 1;
	string text;
	switch (getUndo()->get(row, col, count, text)) { // O(1)
	case Undo::Action::INSERT:
		moveCursor(row, col); // O(abs(m_row - row))
		for (auto it = text.begin(); it != text.end(); ++it) // O(text.size * L)
			insertAtCursor(*it); // O(L)
		break;
	case Undo::Action::DELETE:
		moveCursor(row, col); // O(abs(m_row - row))
		for (int i = 0; i < count; ++i) // O(text.size * L)
			eraseAtCursor(); // O(L)
		break;
	case Undo::Action::SPLIT:
		moveCursor(row, col); // O(abs(m_row - row))
		splitAtCursor(); // O(L)
		break;
	case Undo::Action::JOIN:
		moveCursor(row, col); // O(abs(m_row - row))
		joinAtCursor(); // O(L1 + L2)
		break;
	}
	moveCursor(row, col); // O(abs(m_row - row))
}
