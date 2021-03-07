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
	// TODO
}

StudentTextEditor::~StudentTextEditor()
{
	// TODO
}

void StudentTextEditor::resetCursor() {
	m_curLine = m_lines.begin();
	m_row = 0;
	m_col = 0;
}

// TODO: Bug both here and in sample when loading empty file
bool StudentTextEditor::load(std::string file) {
	ifstream infile(file);
	if (!infile)
		return false;
	reset();
	string line;
	while (getline(infile, line)) {
		line.erase(remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(remove(line.begin(), line.end(), '\n'), line.end());
		m_lines.push_back(line);
		m_numLines++;
	}
	resetCursor();
	return true;
}

bool StudentTextEditor::save(std::string file) {
	ofstream outfile(file);
	if (!outfile)
		return false;
	for (auto it = m_lines.begin(); it != m_lines.end(); ++it)
		outfile << *it << '\n';
	return true;
}

void StudentTextEditor::reset() {
	m_lines.clear();
	resetCursor();
	m_numLines = 0;
	getUndo()->clear();
}

void StudentTextEditor::insert(char ch) {
	string& curLine = *m_curLine;
	if (ch == '\t') {
		curLine.insert(m_col, "    ");
		m_col += 4;
	}
	else {
		curLine.insert(m_col, 1, ch);
		m_col++;
	}
	getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
}

void StudentTextEditor::enter() {
	auto curLinePtr = m_curLine;
	string newLine = curLinePtr->substr(m_col);
	curLinePtr->erase(m_col);
	m_curLine = m_lines.insert(++curLinePtr, newLine);
	getUndo()->submit(Undo::Action::SPLIT, m_row, m_col);
	m_row++;
	m_col = 0;
	m_numLines++;
}

void StudentTextEditor::del() {
	if (m_col < m_curLine->size()) {
		char deletedChar = m_curLine->at(m_col);
		m_curLine->erase(m_col, 1);
		getUndo()->submit(Undo::Action::DELETE, m_row, m_col, deletedChar);
	}
	else if (m_row < m_numLines - 1) {
		auto nextLinePtr = m_curLine;
		string nextLine = *(++nextLinePtr);
		m_curLine->append(nextLine);
		m_lines.erase(nextLinePtr);
		m_numLines--;
		getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
	}
}

void StudentTextEditor::backspace() {
	if (m_col > 0) {
		m_col--;
		char deletedChar = m_curLine->at(m_col);
		m_curLine->erase(m_col, 1);
		getUndo()->submit(Undo::Action::DELETE, m_row, m_col, deletedChar);
	}
	else if (m_row > 0) {
		auto curLinePtr = m_curLine--;
		m_row--;
		m_col = m_curLine->size();
		string curLine = *curLinePtr;
		m_curLine->append(curLine);
		m_lines.erase(curLinePtr);
		m_numLines--;
		getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
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

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	if (startRow < 0 || numRows < 0 || startRow > m_numLines)
		return -1;

	auto startRowPtr = m_curLine;
	int curRow = m_row;
	if (curRow > startRow) {
		while (curRow > startRow) {
			startRowPtr--;
			curRow--;
		}
	}
	else {
		while (curRow < startRow) {
			startRowPtr++;
			curRow++;
		}
	}

	lines.clear();

	int linesCopied = 0;
	while (startRowPtr != m_lines.end() && curRow < m_numLines && linesCopied < numRows) {
		lines.push_back(*startRowPtr);
		startRowPtr++;
		curRow++;
		linesCopied++;
	}

	return linesCopied;
}

void StudentTextEditor::undo() {
	int row, col, count;
	string text;
	switch (getUndo()->get(row, col, count, text)) {
	case Undo::Action::INSERT:
		m_row = row;
		m_col = col;
		for (auto it = text.begin(); it != text.end(); ++it)
			insert(*it);
		break;
	case Undo::Action::DELETE:
		m_row = row;
		m_col = col;
		for (int i = 0; i < count; ++i)
			del();
		break;
	case Undo::Action::SPLIT:
		m_row = row;
		m_col = col;
		enter();
		m_curLine--;
		break;
	case Undo::Action::JOIN:
		m_row = row;
		m_col = col;
		del();
		break;
	}
	m_row = row;
	m_col = col;
}
