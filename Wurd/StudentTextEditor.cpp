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
 : TextEditor(undo), m_row(0), m_col(0) {
	// TODO
}

StudentTextEditor::~StudentTextEditor()
{
	// TODO
}

bool StudentTextEditor::load(std::string file) {
	ifstream infile(file);
	if (!infile)
		return false;
	string line;
	while (getline(infile, line)) {
		line.erase(remove(line.begin(), line.end(), '\r'), line.end());
		line.erase(remove(line.begin(), line.end(), '\n'), line.end());
		// TODO: Remove \r, \n and store lines
	}
	return true;
}

bool StudentTextEditor::save(std::string file) {
	ofstream outfile(file);
	if (!outfile)
		return false;
	// TODO: Add \n and write lines
	// TODO: endl instead of \n?
	return true;
}

void StudentTextEditor::reset() {
	// TODO: clear text
	m_row = 0;
	m_col = 0;
	// TODO: clear undo
}

void StudentTextEditor::insert(char ch) {
	// TODO: move over chars after cursor
	// TODO: insert char at cursor
	m_col++;
	// TODO: update undo
}

void StudentTextEditor::enter() {
	// TODO: split line at cursor
	// TODO: move second half of line to next line
	m_row++;
	m_col = 0;
	// TODO: update undo
}

void StudentTextEditor::del() {
	// TODO: delete character at cursor, combine with next line, or nothing if invalid
	// TODO: update undo
}

void StudentTextEditor::backspace() {
	// TODO: delete character before cursor, combine with prev line, or nothing if invalid
	// TODO: update cursor
	// TODO: update undo
}

void StudentTextEditor::move(Dir dir) {
	// TODO
}

void StudentTextEditor::getPos(int& row, int& col) const {
	// TODO
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	// TODO
}

void StudentTextEditor::undo() {
	// TODO
}
