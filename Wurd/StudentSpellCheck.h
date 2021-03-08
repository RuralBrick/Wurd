#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

class StudentSpellCheck : public SpellCheck {
public:
	StudentSpellCheck();
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct Node {
		char letter;
		std::vector<Node*> children;
	};
	Node* m_wordTrie;
	Node* createNode(char letter);
	void deleteNode(Node* node);
	void resetTrie();
	Node* giveNextNode(Node* curNode, char letter);
	void printTrie(Node* node) const;
	std::vector<Node*>::iterator findChildWithChar(Node* node, char ch) const;
	bool checkInDict(Node* curNode, std::string::const_iterator wordBegin, std::string::const_iterator wordEnd) const;
	std::vector<std::string> findSuggestions(Node* curNode, std::string curSuggestion,
		std::string::const_iterator wordBegin, std::string::const_iterator wordEnd, bool foundDiffer) const;
};

#endif  // STUDENTSPELLCHECK_H_
