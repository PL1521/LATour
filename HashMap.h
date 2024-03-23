#ifndef HashMap_h
#define HashMap_h

#include <iostream>
#include <vector>
#include <list>
#include <string>

template <typename T>
class HashMap
{
  public:
    HashMap(double max_load = 0.75) // constructor
    : m_entries(0), m_buckets(10)
    {
        if (max_load < 0)
            m_load = 0.75;
        m_load = max_load;
        table.resize(m_buckets);
    }
    
    ~HashMap() {} // destructor; deletes all of the items in the hashmap
    
    int size() const // return the number of associations in the hashmap
    {
        return m_entries;
    }
    
    // The insert method associates one item (key) with another (value).
    // If no association currently exists with that key, this method inserts
    // a new association into the hashmap with that key/value pair. If there is
    // already an association with that key in the hashmap, then the item
    // associated with that key is replaced by the second parameter (value).
    // Thus, the hashmap must contain no duplicate keys.
    void insert(const std::string& key, const T& value)
    {
        T* tPtr = find(key);
        //Key doesn't exist in the table ==> add the key/value pair
        if (tPtr == nullptr)
        {
            m_entries++;
            double cLoad = static_cast<double>(m_entries) / m_buckets;
            //Create a new container with double the # of buckets and rehash all items from the original container
            if (cLoad > m_load)
                rehash();
                
            //Add a node to represent the key/value pair
            size_t h = std::hash<std::string>()(key);
            table[h % m_buckets].emplace_back(key, value);
        //Key exists in the table ==> update the value of the key/value pair
        } else
        {
            *tPtr = value;
        }
    }
    
    // Defines the bracket operator for HashMap, so you can use your map like this:
    // your_map["david"] = 2.99;
    // If the key does not exist in the hashmap, this will create a new entry in
    // the hashmap and map it to the default value of type T (0 for builtin types).
    // It returns a reference to the newly created value in the map.
    T& operator[](const std::string& key)
    {
        T* temp = find(key);
        if (temp == nullptr)
            insert(key, T());
        return *find(key);
    }
    
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value within the map.
    const T* find(const std::string& key) const
    {

        size_t h = std::hash<std::string>()(key);
        int idx = h % m_buckets;
        typename std::list<Node>::const_iterator it = table[idx].begin();
        while (it != table[idx].end())
        {
            if ((*it).key == key)
                return &((*it).val);
            it++;
        }
        return nullptr;
    }
    
    // If no association exists with the given key, return nullptr; otherwise,
    // return a pointer to the value associated with that key. This pointer can be
    // used to examine that value or modify it directly within the map.
    T* find(const std::string& key) {
        const auto& hm = *this;
        return const_cast<T*>(hm.find(key));
    }
    
    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;
    
  private:
    struct Node
    {
        std::string key;
        T val;
        Node(const std::string &k, const T &v) : key(k), val(v) {}
    };
    std::vector<std::list<Node>> table;
    int m_entries;
    int m_buckets;
    double m_load;
    void rehash()
    {
        int prev_buckets = m_buckets;
        m_buckets *= 2;
        std::vector<std::list<Node>> tempTable(m_buckets);
        for (int i = 0; i < prev_buckets; i++)
        {
            if (table[i].size() == 0)
                continue;
            typename std::list<Node>::iterator itr = table[i].begin();
            while (itr != table[i].end())
            {
                size_t newIndex = std::hash<std::string>()((*itr).key) % m_buckets;
                tempTable[newIndex].emplace_back((*itr).key, (*itr).val);
                itr++;
            }
        }
        //Set the original container to the new container
        table.swap(tempTable);
    }
};

#endif /* HashMap_h */
