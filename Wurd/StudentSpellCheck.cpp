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
	auto isNotAlphaApos = [](char ch) { return !(isalpha(static_cast<unsigned char>(ch)) || ch == '\''); };
	string word;
	while (getline(infile, word)) {
		word.erase(remove_if(word.begin(), word.end(), isNotAlphaApos), word.end());
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

inline
std::vector<StudentSpellCheck::Node*>::iterator StudentSpellCheck::findChildWithChar(Node* node, char ch) const {
	return find_if(node->children.begin(), node->children.end(), [ch](Node* n) { return n->letter == ch; });
}

bool StudentSpellCheck::checkInDict(Node* curNode, std::string::const_iterator wordBegin, std::string::const_iterator wordEnd) const {
	if (wordBegin == wordEnd) {
		if (findChildWithChar(curNode, '.') == curNode->children.end())
			return false;
		else
			return true;
	}
	char curLetter = tolower(*wordBegin);
	auto nextNodeItr = findChildWithChar(curNode, curLetter);
	if (nextNodeItr == curNode->children.end())
		return false;
	return checkInDict(*nextNodeItr, wordBegin + 1, wordEnd);
}

std::vector<std::string> StudentSpellCheck::findSuggestions(Node* curNode, std::string curSuggestion,
	std::string::const_iterator wordBegin, std::string::const_iterator wordEnd, bool foundDiffer) const {
	vector<string> suggestions;
	if (wordBegin == wordEnd || curNode->letter == '.') {
		if (findChildWithChar(curNode, '.') != curNode->children.end())
			suggestions.push_back(curSuggestion);
		return suggestions;
	}

	char curLetter = tolower(*wordBegin);
	if (!foundDiffer) {
		for (auto child : curNode->children) {
			if (child->letter != curLetter) {
				vector<string> differSuggestions(findSuggestions(child, curSuggestion + child->letter, wordBegin + 1, wordEnd, true));
				suggestions.insert(suggestions.end(), differSuggestions.begin(), differSuggestions.end());
			}
		}
	}

	auto nextNodeItr = findChildWithChar(curNode, curLetter);
	if (nextNodeItr != curNode->children.end()) {
		vector<string> matchSuggestions(findSuggestions(*nextNodeItr, curSuggestion + curLetter, wordBegin + 1, wordEnd, foundDiffer));
		suggestions.insert(suggestions.end(), matchSuggestions.begin(), matchSuggestions.end());
	}

	return suggestions;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	if (checkInDict(m_wordTrie, word.begin(), word.end()))
		return true;
	suggestions = findSuggestions(m_wordTrie, "", word.begin(), word.end(), false);
	if (suggestions.size() > max_suggestions)
		suggestions.resize(max_suggestions);
	return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	problems.clear();
	auto isAlphaApos = [](char ch) { return isalpha(static_cast<unsigned char>(ch)) || ch == '\''; };
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
				newPos.end = wordEnd - 1;
				problems.push_back(newPos);
			}
			wordStart = wordEnd;
		}
	}
}
