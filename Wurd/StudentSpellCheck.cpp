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
		// Return matching child if found
		if (child->letter == letter)
			return child;
	// Else create and add new node
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
	while (getline(infile, word)) { // O(N)
		// Remove and erase all non alphabet nor apostrophe chars
		word.erase(remove_if(word.begin(), word.end(), isNotAlphaApos), word.end()); // O(1) (bounded word length)
		if (word.size() >= 1) {
			// Trace trie until all letters inserted
			char curLetter = tolower(word.front());
			Node* curNode = giveNextNode(m_wordTrie, curLetter); // O(1) (bounded word length)
			for (auto it = word.begin() + 1; it != word.end(); ++it) { // O(1) (bounded word length)
				curLetter = tolower(*it);
				curNode = giveNextNode(curNode, curLetter);
			}
			// Add word terminator
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
	// If word has ended:
	if (wordBegin == wordEnd) {
		// Check if there is a terminator
		if (findChildWithChar(curNode, '.') == curNode->children.end()) // O(curNode->children.size)
			return false;
		else
			return true;
	}
	// Else try to find next letter in trie
	char curLetter = tolower(*wordBegin);
	auto nextNodeItr = findChildWithChar(curNode, curLetter); // O(curNode->children.size)
	if (nextNodeItr == curNode->children.end())
		// Return if next letter is not found
		return false;
	// Continue search
	return checkInDict(*nextNodeItr, wordBegin + 1, wordEnd);
}

std::vector<std::string> StudentSpellCheck::findSuggestions(Node* curNode, std::string curSuggestion,
	std::string::const_iterator wordBegin, std::string::const_iterator wordEnd, bool foundDiffer) const {

	vector<string> suggestions;
	if (wordBegin == wordEnd || curNode->letter == '.') {
		// If word has ended and there is a terminator:
		if (findChildWithChar(curNode, '.') != curNode->children.end()) // O(curNode->children.size)
			// Add current suggested word to suggestion vector
			suggestions.push_back(curSuggestion);
		// Return empty vector if word has ended without terminator
		// Or word hasn't ended and currently at a terminator
		return suggestions;
	}

	char curLetter = tolower(*wordBegin);
	auto nextNodeItr = findChildWithChar(curNode, curLetter);
	if (nextNodeItr != curNode->children.end()) {
		// Get suggestions from child matching current letter
		vector<string> matchSuggestions(findSuggestions(*nextNodeItr, curSuggestion + curLetter, wordBegin + 1, wordEnd, foundDiffer));
		suggestions.insert(suggestions.end(), matchSuggestions.begin(), matchSuggestions.end()); // O(matchSuggestions.size)
	}
	if (!foundDiffer) {
		// Get wildcard suggestions from children
		for (auto child : curNode->children) {
			if (child->letter != curLetter) {
				vector<string> differSuggestions(findSuggestions(child, curSuggestion + child->letter, wordBegin + 1, wordEnd, true));
				suggestions.insert(suggestions.end(), differSuggestions.begin(), differSuggestions.end()); // O(differSuggestions.size)
			}
		}
	}

	return suggestions;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	if (checkInDict(m_wordTrie, word.begin(), word.end())) // O(L)
		return true;
	suggestions = findSuggestions(m_wordTrie, "", word.begin(), word.end(), false); // O(L^2)
	if (suggestions.size() > max_suggestions)
		suggestions.resize(max_suggestions); // O(suggestions.size - max_suggestions)
	return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	problems.clear();
	auto isNotAlphaApos = [](char ch) { return !(isalpha(static_cast<unsigned char>(ch)) || ch == '\''); };
	int wordStart = 0;
	int wordEnd = 0;
	int lineEnd = line.size();
	while (wordStart < lineEnd) { // O(S + W * L)
		char wordFront = line.at(wordStart);
		if (isNotAlphaApos(wordFront))
			// Set wordStart to first letter/apostrophe
			wordStart++;
		else {
			wordEnd = wordStart;
			// Move wordEnd to first non alphabet/apostrophe char after wordStart
			while (wordEnd < lineEnd) { // O(L)
				char wordBack = line.at(wordEnd);
				if (isNotAlphaApos(wordBack))
					break;
				wordEnd++;
			}
			// If word is not in dictionary:
			if (!checkInDict(m_wordTrie, line.begin() + wordStart, line.begin() + wordEnd)) { // O(L)
				// Add position to problems
				SpellCheck::Position newPos;
				newPos.start = wordStart;
				newPos.end = wordEnd - 1;
				problems.push_back(newPos);
			}
			wordStart = wordEnd;
		}
	}
}
