#include <vector>
#include <map>


template <class ValueType, class KeyType>	
class MinHeapUnit {
public:
	ValueType value;
	KeyType key;

	MinHeapUnit() {}
	MinHeapUnit(ValueType v, KeyType k): value(v), key(k) {}
};

template <class ValueType, class KeyType>
class MinHeap {
protected:
	std::vector< MinHeapUnit<ValueType, KeyType> > queue;
	std::map<KeyType, int> key2pos;
	int heapSize;
	
	void MinHeapifyUp(int curr) {
		MinHeapUnit<ValueType, KeyType> temp;
		int parent = (curr - 1) >> 1;
		while (curr > 0 && queue[parent].value > queue[curr].value) {
			key2pos[queue[curr].key] = parent;
			key2pos[queue[parent].key] = curr;
			temp = queue[curr], queue[curr] = queue[parent], queue[parent] = temp;
			curr = parent, parent = (curr - 1) >> 1;
		}
	}

	void MinHeapifyDown(int curr) {
		MinHeapUnit<ValueType, KeyType> temp;
		int left = (curr << 1) + 1, right = left + 1;
		int less = (left < heapSize && queue[left].value < queue[curr].value) ? left : curr;
		if (right < heapSize && queue[right].value < queue[less].value) less = right;
		while (less != curr) {
			key2pos[queue[curr].key] = less;
			key2pos[queue[less].key] = curr;
			temp = queue[curr], queue[curr] = queue[less], queue[less] = temp;
			curr = less;
			left = (curr << 1) + 1, right = left + 1;
			less = (left < heapSize && queue[left].value < queue[curr].value) ? left : curr;
			if (right < heapSize && queue[right].value < queue[less].value) less = right;
		}
	}

public:
	MinHeap(): heapSize(0) {
		key2pos.clear();
	}

	bool Empty() const {
		return heapSize == 0;
	}

	int GetSize() const {
		return heapSize;
	}

	void Reset() {
		queue.clear();
		key2pos.clear();
		heapSize = 0;
	}

	int Find(const KeyType& key)
	{
		int pos = key2pos[key];
		if (pos < 0 || pos >= heapSize || queue[pos].key != key) return -1;
		else return pos;
	}

	ValueType GetValue(const KeyType& key)
	{
		return queue[key2pos[key]].value;
	}

	int ExtractMin(KeyType& key, ValueType& value) {
		if (heapSize <= 0) return -1;
		key = queue[0].key, value = queue[0].value;
		key2pos.erase(queue[0].key);
		if (heapSize > 1) {
			key2pos[queue[heapSize - 1].key] = 0;
			queue[0] = queue[heapSize - 1];
		}
		queue.pop_back();
		-- heapSize;
		MinHeapifyDown(0);
		return 0;
	}
	
	int UpdateKey(const KeyType& key, const ValueType& value) {
		int pos = key2pos[key], parent = (pos - 1) >> 1;
		if (pos < 0 || pos >= heapSize || queue[pos].key != key) return -1;
		queue[pos].value = value;
		if (pos > 0 && queue[pos].value < queue[parent].value) MinHeapifyUp(pos);
		else MinHeapifyDown(pos);
		return 0;
	}

	void Insert(const KeyType& key, const ValueType& value) {
		MinHeapUnit<ValueType, KeyType> temp(value, key);
		queue.push_back(temp);
		key2pos[key] = heapSize;
		++heapSize;
		MinHeapifyUp(heapSize - 1);	
	}

	int Delete(const KeyType& key) {
		int pos = key2pos[key], parent = (pos - 1) >> 1;
		if (pos < 0 || pos >= heapSize || queue[pos].key != key) return -1;
		key2pos.erase(key);
		if (pos < heapSize - 1) {
			key2pos[queue[heapSize - 1].key] = pos;
			queue[pos] = queue[heapSize - 1];
		}
		queue.pop_back();
		-- heapSize;
		if (pos > 0 && queue[pos].value < queue[parent].value) MinHeapifyUp(pos);
		else MinHeapifyDown(pos);		
		return 0;
	}
};