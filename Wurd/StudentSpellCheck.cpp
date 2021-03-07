#include "StudentSpellCheck.h"
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <fstream>
using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::StudentSpellCheck()
	: m_wordTrie(createNode('\0')) {}

StudentSpellCheck::~StudentSpellCheck() {
	deleteNode(m_wordTrie);
}

StudentSpellCheck::Node* StudentSpellCheck::createNode(char letter) const {
	Node* newNode = new Node;
	newNode->letter = letter;
	return newNode;
}

void StudentSpellCheck::deleteNode(Node* node) const {
	if (node == nullptr)
		return;
	for (auto child : node->children)
		deleteNode(child);
	delete node;
}

void StudentSpellCheck::printTrie(Node* node) const {
	cerr << node->letter << ',';
	for (auto child : node->children)
		printTrie(child);
}

void StudentSpellCheck::resetTrie() {
	deleteNode(m_wordTrie);
	m_wordTrie = createNode('\0');
}

StudentSpellCheck::Node* StudentSpellCheck::genNextNode(Node* curNode, char letter) const {
	for (auto child : curNode->children)
		if (child->letter == letter)
			return child;
	Node* newNode = createNode(letter);
	curNode->children.push_back(newNode);
	return newNode;
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	ifstream infile(dictionaryFile);
	if (!infile)
		return false;
	resetTrie();
	string word;
	while (getline(infile, word)) {
		word.erase(remove_if(word.begin(), word.end(), [](char ch) { return !(isalpha(ch) || ch == '\''); }), word.end());
		if (word.size() >= 1) {
			char curLetter = tolower(word.front());
			Node* curNode = genNextNode(m_wordTrie, curLetter);
			for (auto it = word.begin() + 1; it != word.end(); ++it) {
				curLetter = tolower(*it);
				curNode = genNextNode(curNode, curLetter);
			}
			genNextNode(curNode, '.');
		}
	}
	return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	return false; // TODO
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	// TODO
}
