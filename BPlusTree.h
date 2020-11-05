#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "struct.h"
#include<unistd.h>
#include<stdlib.h>
#include<string>
#include<fcntl.h>
#include<iostream>

#define INDEX_FILE_PATH "index_" //索引文件前缀


class BPlusTree {
public:
    //创建一个节点对象
    BPlusTreeNode* BPlusTreeNode_new();

    //创建B+树
    BPlusTreeNode* BPlusTree_create();

    //创建一个索引节点
    IndexNode IndexNode_new(const Record& record, int col);

    //插入未满的节点中
    void BPlusTree_Insert_NotFull(BPlusTreeNode* node, const IndexNode& indexNode);

    //节点分裂
    int BPlusTree_Split_Child(BPlusTreeNode* parent, int pos, BPlusTreeNode* child);

    //插入节点s
    BPlusTreeNode* BPlusTree_Insert(BPlusTreeNode* root, const Record& record, int col);

    //搜索特定属性值
    void SearchValueEqual(BPlusTreeNode* root, int value, int64_t *result, int &num);

    //搜索属性值在[left,right]范围内的记录
    void SearchValueRange(BPlusTreeNode* root, int left, int right, int64_t *result, int &num);

    //保存B+树
    void WriteBPlusTree(BPlusTreeNode* root, int col);

    //将B+树节点写入文件
    void WriteBPlusTreeNode(int &fd, BPlusTreeNode *node);

    //读取B+树
    BPlusTreeNode *ReadBPlusTree(int col);

    //读取B+树节点
    BPlusTreeNode *ReadBPlusTreeNode(int &fd,BPlusTreeNode* &leaf_node_pre);
};









#endif // BPLUSTREE_H
