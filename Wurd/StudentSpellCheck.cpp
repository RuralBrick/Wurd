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

StudentSpellCheck::Node* StudentSpellCheck::createNode(char letter) {
	Node* newNode = new Node;
	newNode->letter = letter;
	return newNode;
}

void StudentSpellCheck::deleteNode(Node* node) {
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

StudentSpellCheck::Node* StudentSpellCheck::giveNextNode(Node* curNode, char letter) {
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
			Node* curNode = giveNextNode(m_wordTrie, curLetter);
			for (auto it = word.begin() + 1; it != word.end(); ++it) {
				curLetter = tolower(*it);
				curNode = giveNextNode(curNode, curLetter);
			}
			giveNextNode(curNode, '.');
		}
	}
	return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	return false; // TODO
}

bool StudentSpellCheck::checkInDict(Node* curNode, std::string::const_iterator wordBegin, std::string::const_iterator wordEnd) const {
	auto begin = curNode->children.begin();
	auto end = curNode->children.end();
	if (wordBegin == wordEnd) {
		if (find_if(begin, end, [](Node* node) { return node->letter == '.'; }) == end)
			return false;
		else
			return true;
	}
	auto curLetter = find_if(begin, end, [wordBegin](Node* node) { return node->letter == tolower(*wordBegin); });
	if (curLetter == end)
		return false;
	return checkInDict(*curLetter, wordBegin + 1, wordEnd);
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	problems.clear();
	auto isAlphaApos = [](char ch) { return isalpha(ch) || ch == '\''; };
	int wordStart = 0;
	int wordEnd = 0;
	int lineEnd = line.size();
	while (wordStart < lineEnd) {
		char wordFront = line.at(wordStart);
		if (!isAlphaApos(wordFront))
			wordStart++;
		else {
			wordEnd = wordStart;
			while (wordEnd < lineEnd) {
				char wordBack = line.at(wordEnd);
				if (!isAlphaApos(wordBack))
					break;
				wordEnd++;
			}
			if (!checkInDict(m_wordTrie, line.begin() + wordStart, line.begin() + wordEnd)) {
				SpellCheck::Position newPos;
				newPos.start = wordStart;
				newPos.end = wordEnd;
				problems.push_back(newPos);
			}
			wordStart = wordEnd;
		}
	}
}
