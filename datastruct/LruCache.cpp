#include<cstdlib>
#include<iostream>
#include<string.h>
/*����ΪLruCache��*/
using namespace std;
const int hashTableSize = 97;//hashͰ������
template<class K, class V>
struct hash_Node //����hash�ڵ�Ľṹ��
{
    K key;
    V value;
    hash_Node *next;
    hash_Node(K k1, V v1):key(k1),value(v1),next(nullptr) {}
    hash_Node(const hash_Node&);
    hash_Node& operator=(const hash_Node&);
};

template<class K, size_t tablesize>
struct KeyHash //����hash����
{
    unsigned long operator()(const K& key) const //���أ���
    {
        return static_cast<unsigned long>(key)%tablesize;
    }
};
template<class K, class V,size_t table_size, class F = KeyHash<K,table_size>>
class hash_table
{
private:
    hash_Node<K,V>* table[table_size];//��ϣͰ�ĸ���
    F hashFunc;//��ϣ����
    hash_table& operator=(const hash_table& other);
public:
    hash_Node<K,V>* operator [](const K& k1) //����Ҫ���أ�Ҫ��Ȼtable[i]�ᱨ��
    {
        unsigned long hashK = hashFunc(k1);
        return table[hashK];
    }
    hash_table():table(),hashFunc()
    {
    }
    ~hash_table() //��������
    {
        cout<<"hash_table begin destruct"<<endl;
        for(size_t i = 0; i < table_size; i++)
        {
            hash_Node<K,V> *node = table[i];

            while(node != nullptr)
            {
                hash_Node<K, V> *prev = node;
                node = node->next;
                delete prev;
            }
            table[i] = nullptr;
        }
    }
    /*brief: hash_table ��ȡĳ��Ԫ��
    *param: k1 �����k������K�ҵ�V
    *return: V Ҳ����hash_table��val��
    *author: �ﺺ�� data:2019-09-02
    */
    V get(const K& k1) //���ݴ����key������alue
    {
        //1������key��hashֵ
        unsigned long hashkey = hashFunc(k1);
        hash_Node<K, V> *node = table[hashkey];
        while(node != nullptr)
        {
            if(node->key == k1) //������򷵻�
            {
                return node->value;
            }
            node = node->next;
        }
        return nullptr;//û�з��ؿ�ֵ
    }
    /*brief: hash_table ����ĳ��Ԫ��
    *param: k1 �����key��v1:�����value
    *author: �ﺺ�� data:2019-09-02
    */
    void put(const K& k1, const V& v1) //�����������滻���е�key
    {
        //1�ȼ���hash
        unsigned long hashK = hashFunc(k1);
        //2������hash�ҵ�Ԫ��
        hash_Node<K, V> *node = table[hashK];
        while(node != nullptr) //�ȱ�����û���ظ��Ľ�㣬�ظ��Ľ����滻
        {
            if(node->key == k1)
            {
                node->value = v1;//�ҵ��˾Ͱ�����Ϊ��ǰ��value
                return;
            }
            node = node->next;
        }
        //���������Ǵ���ͷ����,hash_table�����õ�ͷ���뷨��Ҳ������ν�Ŀ�����
        hash_Node<K, V> *tmp = new hash_Node<K,V>(k1,v1);
        tmp->next = table[hashK];//�ȵ����½�����
        table[hashK] = tmp;//�ٵ����������
    }
    /*brief: hash_table�Ƴ�ĳ��Ԫ��
    *param: k1 �����k������K�ҵ�V
    *author: �ﺺ�� data:2019-09-02
    */
    void remove(const K& k1)
    {
        unsigned long hashK = hashFunc(k1);//��keyd��hashֵ
        hash_Node<K, V> *node = table[hashK];
        hash_Node<K, V> *prev = nullptr;//Ҫɾ���ڵ��ǰ���ڵ�
        //���node�ڵ��Ƿ�Ϊ��
        if(node == nullptr)//Ҳ���Ǹտ�ʼ��û���ҵ�
            return;//����տ�ʼ���ǿգ���zֱ�ӷ���
        while(node != nullptr && node->key != k1) //�ڵ㲻Ϊ�գ����ҵ�ǰ�ڵ��key�����
        {
            prev = node;
            node = node->next;//Ѱ����һ���ڵ�
        }
        if(node == nullptr)//���������һ��keyû���ҵ�
            return;//˵��û���ҵ���Ԫ��
        else
        {
            if(prev == nullptr)//˵����Ԫ�ؾ���Ҫ�ҵĵ�
                table[hashK] = node->next;//ֱ���ÿ�ʼָ��Ҫ�ҵ���һ���ڵ�
            else
            {
                prev->next = node->next;//��ǰ���ڵ�ֱ��ָ��Ҫɾ���ڵ����һ���ڵ�
            }
            delete node;
            node = nullptr;
        }
    }
};

template<class K, class V>
class LRUCache
{
private:
    struct ListNode
    {
        K key;
        V val;
        ListNode *prev;//ǰ�����
        ListNode *next;//��̽ڵ�
        ListNode():key(0),val(0),prev(nullptr),next(nullptr) {};
        ListNode(K k, V v):key(k),val(v),prev(nullptr),next(nullptr) {};
    };
    int capacity;//LRUCache������
    ListNode* dummyNode;//˫�������ͷ������
    hash_table<K,ListNode*,hashTableSize> hashMap;//�Զ����hash_table���
    int tmpSize;//��־LRUCache�ĵ�ǰ�ڵ���

private:
    /*���¼������������ᱻ�����ã��������Ϊ˽��*/

    /*brief:������ͷ����ڵ�
    *param node:����ڵ��Ԫ��ֵ��flag:��־ 0������Ǵ�moveToHead���õģ�����Ҫ������tmpSize�Ĵ�С��������Ҫ����
    *author:�ﺺ�� data:2019-09-03
    */
    void pushFront(ListNode* node,int flag)
    {
        if(node->prev == dummyNode)//���������λ�ã�ֱ�ӷ���
            return;
        node->next = dummyNode->next;//˫��������뾭���Ĳ���
        node->prev = dummyNode;
        dummyNode->next->prev = node;
        dummyNode->next = node;
        if(flag != 0)
            tmpSize++;
    }
    /*brief:�ѵ�ǰ�ڵ�������ж�����Ȼ���ٲ��뵽��һ��λ��
    *author:�ﺺ�� data:2019-09-03
    */
    void moveToHead(ListNode* node)
    {
        if(node->prev == dummyNode)
            return ;
        node->next->prev = node->prev;//˫����ɾ������������
        node->prev->next = node->next;
        pushFront(node,0); //�ѽڵ���뵽����ͷ��,����ط�����pushFront����flag��������0
    }
    /*brief:����������������һ��λ�ã�������ѭ����������λ��Ϊ dummyNode->prev
    *author:�ﺺ�� data:2019-09-03
    */
    void popBack()
    {
        ListNode* tem = dummyNode->prev;
        hashMap.remove(tem->key);//hashMap��Ҳɾ����Ӧ�Ľڵ�
        tem->prev->next = dummyNode;//ɾ��������
        dummyNode->prev = tem->prev;
        tmpSize--;
        delete tem;
    }

public:

    /*brief: LRUCache�Ĺ��캯��
    *param: cap:����ĳ�ʼ���������Ĵ�С
    *author:  �ﺺ�� data:2019-09-03
    */
    LRUCache(int cap)
    {
        capacity = cap;
        tmpSize = 0;
        //��ʼ��dummyNode�ڵ㣬ǰ���ͺ�̶�ָ�����Լ���
        dummyNode = new ListNode();
        dummyNode->prev = dummyNode;
        dummyNode->next = dummyNode;
    }
    /*brief: LRUCache��������
    *author:  �ﺺ�� data:2019-09-03
    */
    ~LRUCache()
    {
        cout<<"LRUcache begin destruct"<<endl;
        eraseAll();
    }
    /*brief: LRUCache����K��ȡV�ĺ���
    *param: key��Ҫ��ѯ������ֵ�Ĵ�С
    *author:  �ﺺ�� data:2019-09-02
    */
    V get(const K& key)
    {
        ListNode*  tmpNode = hashMap.get(key);
        if(tmpNode == nullptr)
            throw "Cache not found element!";
        V value = tmpNode->val;
        moveToHead(tmpNode);//Ԫ���ƶ�����λ��
        return value;
    }
    /*brief: LRUCache����Ԫ�صĺ���
    *param: key1������ֵ�� val1 ֵ����
    *author:  �ﺺ�� data:2019-09-02
    */
    void put(const K& key1, const V& val1)
    {
        ListNode* tmpNode = hashMap.get(key1);
        if(tmpNode == nullptr)
        {
            if(tmpSize == capacity)//�����ǰ�ڵ��������
            {
                //�Ƴ�����Ԫ��
                popBack();
            }
            //ͷ����Ԫ��
            ListNode* frontNode = new ListNode(key1,val1);
            pushFront(frontNode,1);//��������ͷ���
            hashMap.put(key1,frontNode);//����hashMa��
        }
        else//˵��ԭ������Ԫ��
        {
            tmpNode->val = val1;//val1ֵ�п����ǲ�ͬ��
            hashMap.put(key1,tmpNode);//�����Ԫ�أ����滻��Ӧ��val
            moveToHead(tmpNode);
        }
    }
    /*brief: LRUCache�б���Ԫ�صĺ���
    *author:  �ﺺ�� data:2019-09-02
    */
    void showElement()
    {
        ListNode *tmp = dummyNode->next;
        for(int i = 0 ; i < tmpSize && tmp != nullptr; i++)
        {
            cout<<tmp->val<<" ";
            tmp = tmp->next;
        }
        cout<<endl;
    }
    /*brief: LRUCache��ɾ������Ԫ�صĺ���
    *author:  �ﺺ�� data:2019-09-02
    */
    void eraseAll()
    {
        while(dummyNode->next != dummyNode)
        {
            popBack();
        }
        tmpSize = 0;
    }
    /*brief:�õ�LRUCache��ǰ����Ĵ�С
    return��Lrucache�е�ǰԪ�ظ�����const
    *author:�ﺺ�� data:2019-09-02
    */
    int getSize() const
    {
        return tmpSize;
    }
    /*brief:�õ�LRUCache��ǰ�����Ĵ�С
    return��Lrucache�е�ǰ������С��const
    *author:�ﺺ�� data:2019-09-02
    */
    int getCapacity() const
    {
        return capacity;
    }
};
int main()
{
    LRUCache<int,int> lruCache(5);
    lruCache.put(1,1);
    lruCache.put(2,2);
    lruCache.put(3,3);
    lruCache.put(4,4);
    lruCache.put(5,5);
    lruCache.put(6,6);
    cout<<"�������е�Ԫ��"<<endl;
    lruCache.showElement();
    cout<<"����4�ڵ㣬4�ڵ���ǰ"<<endl;
    lruCache.get(4);
    lruCache.showElement();
    cout<<"���Ԫ�غ�Ĵ�С"<<endl;
    lruCache.eraseAll();
    cout<<lruCache.getSize()<<endl;
    cout<<"ִ����ϣ�������������"<<endl;
    return 0;
}
