
// NOTE: PLEASE RUN THIS CODE USING C++11

//IMPORTANT: before submitting your source code, please make sure to comment your main function (needed for automated testing)

#include <iostream>
#include <string>
#include <map>
#include <vector> //or #include <list>
#include <iterator>
#include <queue>

// do not use using namespace std

// do not alter the struct declaration
struct hufftreenode
{
	char character;
	int frequency;
	hufftreenode *left;
	hufftreenode *right;
};

// do not alter the typedef
typedef hufftreenode *hufftreeptr;

// you can define and use any additional *structures* if you might need to

// YOUR CODE HERE FOR ADDIIONAL STRUCTS DECLARATION, IF ANY
struct Compare
{
	bool operator()(const hufftreeptr &a, const hufftreeptr &b)
	{
		return a->frequency > b->frequency;
	}
};

/////////////////////////////////////////
// Functions declarations (prototypes) //
/////////////////////////////////////////

// do not alter these functions declarations

//utility function
bool valid_hufftree(const hufftreeptr hufftree);
// returns true if the input hufftree is a valid Huffman tree
// i.e. all the terminal nodes (leaves) have characters,
// all the non-leaf nodes have two sub-trees each,
// and the occurrence frequency of a non-leaf node equals
// the sum of the occurrence frequencies of its two sub-trees.

std::string huffencode(const std::string &message, std::map<char, int> &freqtable, hufftreeptr &hufftree, std::map<char, std::string> &hufftable);
// encodes an input message (a string of characters) into an encoded message (string of bits) using the Huffman coding
// builds the Huffman tree and its associated encoding table (i.e. the association between the characters in the tree leaves and their encodings (string of bits))

std::string huffdecode(const std::string &encodedmessage, const hufftreeptr &hufftree);
// decodes an encoded message (a string of bits) into the original message (a string of characters) using the Huffman tree built during the encoding

// you can define and use any additional *functions* if you might need to

// YOUR CODE HERE FOR ADDIIONAL FUNCTIONS DECLARATION (NOT THE IMPLEMENTATION), IF ANY
void updateDecodedmessage(const hufftreeptr &hufftree, std::string &decodedmessage, const std::string &encodedmessage, int &i);
void updateHuffTable(const hufftreeptr &hufftree, std::string bitstring, std::map<char, std::string> &hufftable);
hufftreeptr createSubtree(const hufftreeptr &left_child, const hufftreeptr &right_child);
bool isLeaf(const hufftreeptr &node);
///////////////////
// main function //
///////////////////

//IMPORTANT (Reminder): before submitting your source code, please make sure to comment your main function (needed for automated testing)
int main()
{

	// YOUR CODE HERE FOR TESTING YOUR PROGRAM

	// an example of basic encoding/decoding
	std::string message, encodedmessage, decodedmessage;
	hufftreeptr hufftree;
	std::map<char, int> freqtable;
	std::map<char, std::string> hufftable;

	message = "go go gophers";

	// 1) encoding (compression)
	encodedmessage = huffencode(message, freqtable, hufftree, hufftable);
	// freqtable should be as in Table 1.
	// hufftree might be as in Figure 2 (remember, the Huffman coding tree IS NOT UNIQUE. If we swap two siblings in a Huffman tree, it is still a Huffman tree of the same message.)
	// hufftable should correspond to hufftree, and might be as in Table 2 <' ',"100">, <'g',"00">, <'o',"01">, <'p',"1110">, <'h',"1101">, <'e',"101">, <'r',"1111">, <'s',"1100">.
	// encodedmessage might then be the 37-bits string "0001100000110000011110110110111111100" (i.e. "00.01.100.00.01.100.00.01.1110.1101.101.1111.1100")

	if (valid_hufftree(hufftree))
	{
		std::cout << "valid Huffman tree." << std::endl;
	}
	else
	{
		std::cout << "not valid Huffman tree." << std::endl;
	}

	// 2) decoding (uncompression)
	decodedmessage = huffdecode(encodedmessage, hufftree);
	// should be "go go gophers"

	if (decodedmessage == message)
	{
		std::cout << "decoding OK." << std::endl;
	}
	else
	{
		std::cout << "decoding not OK." << std::endl;
	}

	return 0;
}

/////////////////////////////////////////////
// Functions definitions (implementations) //
/////////////////////////////////////////////

// YOUR CODE HERE FOR THE FUNCTIONS IMPLEMENTATIONS

// returns true if the input hufftree is a valid Huffman tree
// i.e. all the terminal nodes (leaves) have characters,
// all the non-leaf nodes have two sub-trees each,
// and the occurrence frequency of a non-leaf node equals
// the sum of the occurrence frequencies of its two sub-trees.
bool valid_hufftree(const hufftreeptr hufftree)
{
	if (isLeaf(hufftree))
	{
		// If leaf node fulfills leaf conditions
		return true;
	}
	else if (hufftree->left != NULL && hufftree->right != NULL &&
			 hufftree->frequency == (hufftree->left->frequency + hufftree->right->frequency))
	{
		// If non-leaf node and has left and right branches (valid node)
		return true && valid_hufftree(hufftree->left) && valid_hufftree(hufftree->right);
	}
	else
	{
		// If node is invalid
		return false;
	}
}

// encodes an input message (a string of characters) into an encoded message (string of bits) using the Huffman coding
// builds the Huffman tree and its associated encoding table (i.e. the association between the characters in the tree leaves and their encodings (string of bits))
std::string huffencode(const std::string &message, std::map<char, int> &freqtable, hufftreeptr &hufftree, std::map<char, std::string> &hufftable)
{
	// Message legitimacy test
	if (message.empty())
	{
		std::cout << "<< ERROR: EMPTY MESSAGE >>" << '\n';
		exit(EXIT_FAILURE);
	}
	std::string tmp_message = "";
	// 1. Generate the frequency Table
	for (auto &c : message)
	{
		freqtable[c]++;
	}
	// 2. Generate the forest of hufftreenodes
	// Forest (priority_queue, minheap) of hufftreenodes, Logarithmic insert.
	std::priority_queue<hufftreeptr, std::vector<hufftreeptr>, Compare> forest;
	for (auto &it : freqtable)
	{
		hufftreeptr leaf = new hufftreenode;
		leaf->character = it.first;
		leaf->frequency = it.second;
		leaf->left = NULL;
		leaf->right = NULL;
		forest.push(leaf);
	}
	// 3. Generate the huffman tree
	if (forest.size() == 1)
	{ // Special case, only one element in the message
		hufftree = forest.top();
		return tmp_message;
	}
	while (forest.size() > 1)
	{ // Message has more than one element
		hufftreeptr tmp_left, tmp_right, tmp_root;
		tmp_left = forest.top();
		forest.pop();
		tmp_right = forest.top();
		forest.pop();
		tmp_root = createSubtree(tmp_left, tmp_right);
		forest.push(tmp_root);
		hufftree = tmp_root;
	}
	//  4. Generate the huffman table by traversing the tree from root to leaf for each leaf
	std::string bitstring = "";
	updateHuffTable(hufftree, bitstring, hufftable);
	//  5. Generate the encoded message by concatenating each bitstring
	for (auto &c : message)
	{
		tmp_message += hufftable[c];
	}
	return tmp_message;
}

// Decodes an encoded message (a string of bits) into the original message (a string of characters) using the Huffman tree built during the encoding
std::string huffdecode(const std::string &encodedmessage, const hufftreeptr &hufftree)
{
	std::string decodedmessage = "";
	// Special case: one character only in message
	if (isLeaf(hufftree))
	{
		for (int i = 0; i < hufftree->frequency; i++)
		{
			decodedmessage += hufftree->character;
		}
		return decodedmessage;
	}
	// Normal case
	int i = 0;
	while (i < encodedmessage.size())
	{
		updateDecodedmessage(hufftree, decodedmessage, encodedmessage, i);
	}
	return decodedmessage;
}

// Recursively updates the decoded message
void updateDecodedmessage(const hufftreeptr &hufftree, std::string &decodedmessage, const std::string &encodedmessage, int &i)
{
	// Base case: leaf
	if (isLeaf(hufftree))
	{
		decodedmessage += hufftree->character;
		// Case 1: Branch left
	}
	else if (encodedmessage[i] == '0')
	{
		updateDecodedmessage(hufftree->left, decodedmessage, encodedmessage, ++i);
		// Case 2: Branch right
	}
	else
	{
		updateDecodedmessage(hufftree->right, decodedmessage, encodedmessage, ++i);
	}
}

// Recursively updates the huffman table
void updateHuffTable(const hufftreeptr &hufftree, std::string bitstring, std::map<char, std::string> &hufftable)
{
	// Base case: Leaf
	if (isLeaf(hufftree))
	{
		hufftable.emplace(hufftree->character, bitstring);
		// Branch left and right
	}
	else
	{
		updateHuffTable(hufftree->left, bitstring + "0", hufftable);
		updateHuffTable(hufftree->right, bitstring + "1", hufftable);
	}
}

// Creates subtree with two children
hufftreeptr createSubtree(const hufftreeptr &left_child, const hufftreeptr &right_child)
{
	hufftreeptr parent = new hufftreenode;
	parent->left = left_child;
	parent->right = right_child;
	parent->frequency = left_child->frequency + right_child->frequency;
	return parent;
}

// Returns true if node is a leaf and false otherwise
bool isLeaf(const hufftreeptr &node)
{
	if (node->character != '\0' && node->left == NULL && node->right == NULL)
	{
		return true;
	}
	return false;
}
