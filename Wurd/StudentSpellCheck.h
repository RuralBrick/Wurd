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
	Node* createNode(char letter) const;
	void deleteNode(Node* node) const;
	void resetTrie();
	Node* findNextNode(Node* curNode, char letter) const;
	void printNode(Node* node) const;
};

#endif  // STUDENTSPELLCHECK_H_
