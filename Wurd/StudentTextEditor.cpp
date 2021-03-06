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
	// TODO
}

void StudentTextEditor::move(Dir dir) {
	// TODO
}

void StudentTextEditor::del() {
	// TODO
}

void StudentTextEditor::backspace() {
	// TODO
}

void StudentTextEditor::insert(char ch) {
	// TODO
}

void StudentTextEditor::enter() {
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
