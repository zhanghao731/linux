#include "BPlusTree.h"




//创建一个B+树节点
BPlusTreeNode* BPlusTree::BPlusTreeNode_new() {
    BPlusTreeNode* node = (BPlusTreeNode*)malloc(sizeof(BPlusTreeNode));
    if (node == NULL)
        return NULL;
    for (int i = 0; i < 2 * M - 1; i++)
        node->index_nodes[i].value = -1;
    for (int i = 0; i < 2 * M; i++)
        node->childs[i] = NULL;
    node->num = 0;
    node->is_leaf = true;
    node->prev = NULL;
    node->next = NULL;
    return node;

}



//B+树的创建
BPlusTreeNode* BPlusTree::BPlusTree_create() {
    BPlusTreeNode* node = BPlusTreeNode_new();
    if (NULL == node)
        return NULL;
    node->prev = node;
    node->next = node;
    return node;
}

//创建一个索引节点，即B+树节点中的关键码
IndexNode BPlusTree::IndexNode_new(const Record& record, int col) {
    IndexNode indexNode;
    indexNode.primary_key = record.primary_key;
    indexNode.value = record.record_array[col - 1];
    return indexNode;
}

//节点分裂
int BPlusTree::BPlusTree_Split_Child(BPlusTreeNode* parent, int pos, BPlusTreeNode* child) {
    BPlusTreeNode* new_child = BPlusTreeNode_new();
    if (!new_child)
        return -1;
    new_child->is_leaf = child->is_leaf;
    new_child->num = M - 1;
    for (int i = 0; i < M - 1; i++)
        new_child->index_nodes[i] = child->index_nodes[i + M];
    if (!new_child->is_leaf) {
        for (int i = 0; i < M; i++)
            new_child->childs[i] = child->childs[i + M];
    }
    child->num = M - 1;
    if (child->is_leaf)
        child->num++;//如果是叶节点，保留中间的关键码
    for (int i = parent->num; i > pos; i--)
        parent->childs[i + 1] = parent->childs[i];
    parent->childs[pos + 1] = new_child;
    for (int i = parent->num - 1; i >= pos; i--)
        parent->index_nodes[i + 1] = parent->index_nodes[i];
    parent->index_nodes[pos] = child->index_nodes[M - 1];
    parent->num++;

    //叶节点情况，需要更新指针
    if (child->is_leaf) {
        new_child->next = child->next;
        child->next->prev = new_child;
        child->next = new_child;
        new_child->prev = child;
    }
    return 1;
}



//插入一个未满的节点中
void BPlusTree::BPlusTree_Insert_NotFull(BPlusTreeNode * node, const IndexNode& indexNode) {
    if (node->is_leaf) {
        int pos = node->num;
        while (pos >= 1 && indexNode.value < node->index_nodes[pos - 1].value) {
            node->index_nodes[pos] = node->index_nodes[pos - 1];
            pos--;
        }
        node->index_nodes[pos] = indexNode;
        node->num++;
    } else {
        int pos = node->num;
        while (pos > 0 && indexNode.value < node->index_nodes[pos - 1].value) {
            pos--;
        }

        if (2 * M - 1 == node->childs[pos]->num) {
            BPlusTree_Split_Child(node, pos, node->childs[pos]);
            if (indexNode.value > node->index_nodes[pos].value)
                pos++;
        }
        BPlusTree_Insert_NotFull(node->childs[pos], indexNode);
    }
}

//插入节点
BPlusTreeNode* BPlusTree::BPlusTree_Insert(BPlusTreeNode* root, const Record& record, int col) {
    IndexNode indexNode = IndexNode_new(record, col);
    if (!root) {
        return NULL;
    }
    if (root->num == 2 * M - 1) {
        BPlusTreeNode* node = BPlusTreeNode_new();
        if (!node) {
            return NULL;
        }
        node->is_leaf = false;
        node->childs[0] = root;
        BPlusTree_Split_Child(node, 0, root);
        BPlusTree_Insert_NotFull(node, indexNode);
        return node;
    } else {
        BPlusTree_Insert_NotFull(root, indexNode);
        return root;
    }
}

/** 搜索特定属性值
  * root B+树根节点
  * value 属性值
  * result 结果数组 存放记录的主键
  * num 结果数
  */
void BPlusTree::SearchValueEqual(BPlusTreeNode* root, int value, int64_t *result, int &num) {
    num = 0;
    if (!root)
        return;
    BPlusTreeNode *node = root;
    while (!node->is_leaf) { //不是叶节点，向下搜索
        int pos = 0;
        while (pos < node->num && value > node->index_nodes[pos].value)
            pos++;
        node = node->childs[pos];
    }
    //到达叶节点，顺着next指针往前搜索
    while (node) {
        for (int i = 0; i < node->num && num < MAX_RESULT_NUM; i++) {
            if (node->index_nodes[i].value > value)
                return;
            if (node->index_nodes[i].value == value)
                result[num++] = node->index_nodes[i].primary_key;
        }
        if (num == MAX_RESULT_NUM)
            return ;
        node = node->next;
    }
}

/**搜索属性值在[left,right]范围内的记录
 */
void BPlusTree::SearchValueRange(BPlusTreeNode* root, int left, int right, int64_t *result, int &num) {
    num = 0;

    BPlusTreeNode *node_left = root;
    BPlusTreeNode *node_right = root;

    //往下搜索，到达两个端点所在的叶节点
    while (!node_left->is_leaf) {
        int pos = 0;
        while (pos < node_left->num && left > node_left->index_nodes[pos].value)
            pos++;
        node_left = node_left->childs[pos];
    }
    while (!node_right->is_leaf) {
        int pos = node_right->num;
        while (pos > 0 && right < node_right->index_nodes[pos - 1].value)
            pos--;
        node_right = node_right->childs[pos];
    }

    //移动node_left指针直到node_right
    while (node_left != node_right) {
        if(node_left==NULL)
            std::cout<<"error";
        for (int i = 0; i < node_left->num && num < MAX_RESULT_NUM; i++) {
            if (node_left->index_nodes[i].value >= left)
                result[num++] = node_left->index_nodes[i].primary_key;
        }
        if (num == MAX_RESULT_NUM)
            return ;
        node_left = node_left ->next;
    }

    //node_left和node_right相遇
    for (int i = 0; i < node_left->num && num < MAX_RESULT_NUM; i++) {
        if (left <= node_left->index_nodes[i].value && node_left->index_nodes[i].value <= right)
            result[num++] = node_left->index_nodes[i].primary_key;
    }
}

//保存为第col列属性创建的B+树，创建或更新索引时调用此函数
void BPlusTree::WriteBPlusTree(BPlusTreeNode *root, int col) {
    char index_file_path[20];
    sprintf(index_file_path, "%s%d", INDEX_FILE_PATH, col);
    //删除原来的索引文件
    if (access(index_file_path, F_OK) == 0) {
        if (remove(index_file_path ) == -1) {
            std::cout << "删除失败";
            throw "In BPlusTree::WriteBPlusTree(),remove error";
        }
    }
    int fd = open(index_file_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1)
        throw "In BPlusTree::WriteBPlusTree(),open error";
    WriteBPlusTreeNode(fd, root);
    close(fd);
}


//将B+树节点写入文件
void BPlusTree::WriteBPlusTreeNode(int &fd, BPlusTreeNode *node) {
    if (node == NULL || fd == -1) {
        return ;
    }
    if (write(fd, node, sizeof(BPlusTreeNode)) == -1)
        throw "In BPlusTree::WriteBPlusTreeNode(),write error";
    for (int i = 0; i <= node->num; i++)
        WriteBPlusTreeNode(fd, node->childs[i]);
}

//读取B+树
BPlusTreeNode* BPlusTree::ReadBPlusTree(int col) {
    BPlusTreeNode* leaf_node_pre = NULL; //保存前一个叶节点
    char index_file_path[20];
    sprintf(index_file_path, "%s%d", INDEX_FILE_PATH, col);
    if (access(index_file_path, F_OK) == -1)
        return NULL;
    int fd = open(index_file_path, O_RDONLY, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return NULL;
    BPlusTreeNode *node = ReadBPlusTreeNode(fd, leaf_node_pre);
    close(fd);
    return node;
}

//读取B+树节点
BPlusTreeNode *BPlusTree::ReadBPlusTreeNode(int &fd, BPlusTreeNode* &leaf_node_pre) {

    BPlusTreeNode *node = BPlusTreeNode_new();
    if (node == NULL)
        return NULL;
    if (read(fd, node, sizeof(BPlusTreeNode)) == -1)
        return NULL;
    if (node->is_leaf) {
        if (leaf_node_pre == NULL)
            leaf_node_pre = node;
        else
        {
            leaf_node_pre->next = node;
            leaf_node_pre=node;
        }
        return node;
    }
    for (int i = 0; i <= node->num; i++)
        node->childs[i] = ReadBPlusTreeNode(fd,leaf_node_pre);
    return node;
}
































