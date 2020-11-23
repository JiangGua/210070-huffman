#include "encoder.hpp"

#include <iostream>

using namespace std;

int main(int argc, char const *argv[])
{
  huffman_encode("a.txt");
  return 0;
}

freq_hashmap text_file_to_frequency_hashmap(string filename) {
  freq_hashmap char_freq;
  char ch;
  ifstream fin;
  fin.open(filename.c_str());

  while (fin.get(ch)) {
    char_freq[ch]++;
  }

  fin.close();
  return char_freq;
}

struct MinHeapNode* frequency_to_huffman_tree(freq_hashmap char_freq) {
  // 先将哈希表的数据转换成一个个叶子节点存入优先队列中, 并把低频的放到前面
  // 然后逐一合并节点, 构建 Huffman 树

  // 先创建所有叶子节点
  struct MinHeapNode *left, *right, *top;
  priority_queue<MinHeapNode*, vector<MinHeapNode*>, compare> min_heap;

  // 将哈希表转换成叶子节点
  for (auto itr : char_freq) {
    min_heap.push(new MinHeapNode(itr.first, itr.second));
  }
  
  // 构建 Huffman 树
  while (min_heap.size() != 1) {
    left = min_heap.top();
    min_heap.pop();
    right = min_heap.top();
    min_heap.pop();
    top = new MinHeapNode('$', left->freq + right->freq);
    top->left = left;
    top->right = right;
    min_heap.push(top);
  }

  return min_heap.top();
}

void print_codes(struct MinHeapNode *root, std::string str) {
  if (!root) return;
  if (root->data != '$')
    cout << root->data << " : " << str << "\n";
  print_codes(root->left, str+"0");
  print_codes(root->right, str+"1");
}

unordered_map<char, std::string> huffman_encode_table(struct MinHeapNode *root) {
  unordered_map<char, std::string> result;
  huffman_encode_table_helper(root, result);
  return result;
}

void huffman_encode_table_helper(struct MinHeapNode *root, unordered_map<char, std::string> &result, std::string str) {
  if (!root) return;
  if (root->data != '$')
    result[root->data] = str;
  huffman_encode_table_helper(root->left, result, str+"0");
  huffman_encode_table_helper(root->right, result, str+"1");
}

string huffman_encode_string_to_string(string original) {
  string encoded = "";
  freq_hashmap char_freq = string_to_frequency_hashmap(original);
  struct MinHeapNode* huffman_tree = frequency_to_huffman_tree(char_freq);
  unordered_map<char, std::string> ascii_code_table = huffman_encode_table(huffman_tree);

  for (int i = 0; i < original.size(); i++) {
    encoded += ascii_code_table[original[i]];
  }
  return encoded;
}

freq_hashmap string_to_frequency_hashmap(string text) {
  freq_hashmap char_freq;
  for (int i = 0; i < text.size(); i++) {
    char_freq[text[i]]++;
  }
  return char_freq;
}

void huffman_encode(string filename) {
  string original = "";
  ifstream fin(filename.c_str());
  fin >> original;

  string result = "";
  freq_hashmap char_freq = string_to_frequency_hashmap(original);
  struct MinHeapNode* huffman_tree = frequency_to_huffman_tree(char_freq);
  unordered_map<char, std::string> ascii_code_table = huffman_encode_table(huffman_tree);
  for (int i = 0; i < original.size(); i++) {
    result += ascii_code_table[original[i]];
  }

  ofstream out_huffman_table(filename + ".hct");
  for (auto i: ascii_code_table) {
    out_huffman_table << i.first << " " << i.second << "\n";
  }
  out_huffman_table.close();

  ofstream out_encoded_string(filename + ".hcs");
  out_encoded_string << result;
  out_encoded_string.close();

  //FIXME: 好像是不对的, 正确的做法是8bit 8bit地写
  // ofstream fout(filename + ".hcb", ios::binary);
  // bitset<sizeof(unsigned long) * 8> result_bits(result);
  // unsigned long binary_value = result_bits.to_ulong();
  // fout.write((const char*)&binary_value, sizeof(unsigned long));
  // fout.close();
}
