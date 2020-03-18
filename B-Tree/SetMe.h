#define SETME_H
#ifdef SETME_H
#include<cstdlib>
#include<vector>
//so why the MAXIMUM equals to twice of MINIMUM? Can other value works well?

template<typename T>
class SetMe
{
public:
	typedef T value_type;
	SetMe();
	SetMe(const SetMe& source);
	SetMe operator=(const SetMe& source);
	bool insert(const T& source);
	std::size_t erase(const T& target);
	std::size_t count(const T& target) const;
	bool empty() const { return (data_count == 0); }
	//~SetMe();
	bool loose_remove(const T& entry, std::vector<SetMe<T>*>& path, std::vector<int>& index);

private:
	static const std::size_t MINIMUM = 200;
	static const std::size_t MAXIMUM = 2 * MINIMUM;

	//how many entries a root owns
	std::size_t data_count;
	//store the value the set specipies
	T data[MAXIMUM + 1];
	//avoid to calculate how many elements in array data
	std::size_t child_count;
	//pointer to children
	SetMe<T>* subset[MAXIMUM + 2];
	int* p;
	bool loose_insert(const T& entry,std::vector<SetMe<T>*> &vec,std::vector<int>&);
	bool is_leaf() const { return (child_count == 0); }
	void remove_biggest(T& removed_entry);
	void fix_excess(std::size_t i);
	void fix_shortage(std::size_t i);
};
template<typename T>
SetMe<T>::SetMe() :child_count(0), data_count(0) {};

//��this�ڵ���в�����ȥ��this�Ľڵ���ߵݹ���ñ��
template<typename T>
bool SetMe<T>::loose_remove(const T& target,std::vector<SetMe<T>*>& path,std::vector<int>& index) {
	int i = 0;
	for (i=0; i<data_count && target>data[i]; i++) {};
	//���ҵ����Ԫ�أ��Ϳ���ȥ��Ԫ����
	
	//����ڵ����ӽڵ���δ�ҵ���Ԫ��
	
	if ((data[i]!=target||i==data_count) && child_count == 0) {
		return false;
	}
	//���ӽڵ㣬�ڵ�ǰ�ڵ�û���ҵ�
	else if((data[i] != target || i == data_count)&&child_count!=0) {
		return subset[i]->loose_remove(target, path, index);
	}
	//�ҵ���Ԫ��
	else if (i < data_count && data[i] == target) {
		//����ǰ��,�ӱ�ɾ����Ԫ����ǰshift
		for (int j =i; j<data_count-1; j--) {
			data[j] = data[j + 1];
		}
		--data_count;
		path.push_back(this);
		index.push_back(i);
		return true;
	}
}

template<typename T>
bool SetMe<T>::insert(const T& entry) {
	std::vector<SetMe<T>*> path;
	std::vector<int> index;
	//��ͷ����Ϣ����
	path.push_back(this);
	index.push_back(0);

	//û�гɹ�����loose_insert ����false
	if (!loose_insert(entry,path,index)) { return false; }
	//����ɹ���������
	else{
		//���ȼ���Ƿ񳬱�
		std::size_t len = path.size();
		//ֱ��ѭ�������ã����������������������
		//len-j ��ʾ��ǰ�ڵ㣬��������� len-j-1��ʾ�丸�ڵ㣬�Ը��ڵ㴦��
		int j = 1;
		for (j; path[len -j]->data_count == MAXIMUM + 1 && j < len; j++) {
			SetMe<T>* temp= path[len-j-1];
			temp->fix_excess(index[len-j]);
		}
		//�Զ�������
		if (j == len&&data_count==MAXIMUM+1) {
			//���ʹ������ָ�룿
			SetMe<T>* left=new SetMe<T>();
			SetMe<T>* right = new SetMe<T>();

			//�༭�����ӵ������ڵ�
			for (int i = 0; i < MINIMUM;i++) {
				left->data[i] = this->data[i];
				right->data[i] = this->data[MINIMUM + 1 + i];
				
				left->subset[i] = this->subset[i];
				right->subset[i] = this->subset[MINIMUM + 1 + i];
			}
			left->subset[MINIMUM] = this->subset[MINIMUM];
			right->subset[MINIMUM] = this->subset[MAXIMUM+1];
			
			//���ָ��ڵ㲻�䣬�޸�������
			this->child_count = 2;
			this->data_count = 1;
			this->subset[0] = left;
			this->subset[1] = right;
		}
		return true;
	}
}
//�ڵ��subset[i] ����MAXIMUM��1 ��Ԫ�ص�
//˭���ӽڵ㳬�����˭
template<typename T>
void SetMe<T>::fix_excess(std::size_t i) {
	SetMe<T> node;
	//�����ڵ��Ԫ�صĺ��Ƽ������ӽڵ��м��Ԫ�ص����ʵ�λ��
	for (int j = data_count - i; j > 0; j--) {
		data[j] = data[j-1];
	}
	data[i] = subset[i]->data[MINIMUM];
	
	//���ӽڵ�ֳ����ݣ�nodeΪ�ұߵ�һ����
	node.child_count = MINIMUM+1;
	node.data_count = MINIMUM;
	//��һ��ֱ�Ӽ�ָ�룬���ʹ��󣡣�
	//node.data = subset[i]->data + MINIMUM;

	//�����µ�ֵ���ӽڵ����Ϣ
	for (int j = 0; j < MINIMUM + 1; j++) {
		node.data[j] = subset[i]->data[MINIMUM + j];
		node.subset[j] = subset[i]->subset[MINIMUM + j];
	}

	subset[i]->data_count = MINIMUM;
	subset[i]->child_count = MINIMUM + 1;
}
template<typename T>
bool SetMe<T>::loose_insert(const T& entry,std::vector<SetMe<T>*> &vec,std::vector<int>& index)
{
	std::size_t i;
	//��õ�һ����С�ڲ���ֵ��Ԫ�ص�����
	for (i = 0; entry > data[i] && i < data_count; i++) {};
	//Ӧ�ü������i��data�Ĺ�ϵ�����i��data_count��Ⱦ�˵������ĩβ
	if (entry == data[i] && i < data_count) { return false; }
	else if (child_count == 0) {
		//û���ӽڵ㣬�ݹ�ĳ���
		//��Ҫ������ֵ����
		for (int j = data_count - i; j > i; j--) {
			data[j] = data[j - 1];
		}
		data[i] = entry;
		++data_count;
		return true;
	}
	else {
		vec.push_back(subset[i]);
		index.push_back(i);
		return subset[i]->loose_insert(entry,vec,index);
	}

}

#endif // !SETME_H

//template<typename T>
//class SetMe
//{
//public:
//	typedef T value_type;
//	SetMe();
//	SetMe(const SetMe& source);
//	SetMe operator=(const SetMe& source);
//	bool insert(const T& source);
//	std::size_t erase(const T& target);
//	std::size_t count(const T& target) const;
//	bool empty() cosnt { return (data_count == 0); }
//	~SetMe();
//
//private:
//	static const std::size_t MINIMUM = 200;
//	static const std::size_t MAXIMUM = 2 * MINIMUM;
//
//	//how many entries a root owns
//	std::size_t data_count;
//	//store the value the set specipies
//	T data[MAXIMUM + 1];
//	//avoid to calculate how many elements in array data
//	std::size_t child_count;
//	//pointer to children
//	SetMe* subset[MAXIMUM + 2];
//
//	bool is_leaf() const { return (child_count == 0); }
//	bool loose_insert(const T& entry);
//	bool loose_remove(const T& entry);
//	void remove_biggest(T& removed_entry);
//	void fix_excess(std::size_t i);
//	void fix_shortage(std::size_t i);
//};