#define SETME_H
#ifdef SETME_H
#include<cstdlib>
#include<vector>
#include<queue>
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
	void fix_shortage(std::size_t i);
	bool loose_remove(const T& entry, std::vector<SetMe<T>*>& path, std::vector<int>& index);

	void Show();
private:
	static const std::size_t MINIMUM = 2;
	static const std::size_t MAXIMUM = 2 * MINIMUM;

	//how many entries a root owns
	std::size_t data_count;
	//store the value the set specipies
	T data[MAXIMUM + 1];
	//avoid to calculate how many elements in array data
	std::size_t child_count;
	//pointer to children
	SetMe<T>* subset[MAXIMUM + 2];
	bool loose_insert(const T& entry,std::vector<SetMe<T>*> &vec,std::vector<int>&);
	bool is_leaf() const { return (child_count == 0); }
	void remove_biggest(T& removed_entry);
	void fix_excess(std::size_t i);
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

//˭�ĺ��ӽڵ�����Ԫ�ض�˭������˭�Һ����������Ҫ�Լҳ�����(������
template<typename T>
void SetMe<T>::fix_shortage(std::size_t i) {
	//subset[i]��һ��Ԫ�أ�subset[i-1]�����ṩһ��Ԫ��
	if (subset[i]->data_count == MINIMUM - 1 && subset[i - 1]->data_count > MINIMUM&& i > 0) {
		int j = --subset[i - 1]->data_count;
		T temp = data[i - 1];
		data[i - 1] = subset[i - 1]->data[j];

		//subset[i]Ԫ�غ���
		for (int m = subset[i]->data_count; m > 0; m--) {
			subset[i]->data[m] = subset[i]->data[m - 1];
		}
		subset[i]->data[0] = temp;
	}
	else if (subset[i]->data_count == MINIMUM - 1 && subset[i + 1]->data_count > MINIMUM&& i < child_count - 1) {
		int& j = subset[i]->data_count;
		subset[i]->data[j++] = data[i];
		data[i] = subset[i + 1]->data[0];

		for (int m = 0; m < subset[i + 1]->data_count - 1; m++) {
			subset[i + 1]->data_count[m] = subset[i + 1]->data_count[m + 1];
		}
		--subset[i + 1]->data_count;
	}
	//���кϲ�����,����ߺϲ�,��ʱi����0
	//������߽ڵ㶪���ұ߽ڵ�,�ұ߽ڵ���һ��Ԫ��
	else if (i != 0 && subset[i]->data_count == MINIMUM - 1 && subset[i - 1]->data_count == MINIMUM) {
		T temp = data[i - 1];
		//��this��data��i-1��ʼǰ��
		for (int j = i; j < data_count; j++) {
			data[j - 1] = data[j];
		}
		--data_count;
		int len1 = subset[i - 1]->data_count;
		int len2 = subset[i]->data_count;
		int childsize1 = subset[i - 1]->child_count;
		int childsize2 = subset[i]->child_count;

		subset[i - 1]->data[len1] = temp;
		//���ұ߽ڵ��Ԫ��ֵ�ϲ�����߽ڵ�
		for (int j = 0; j < len2; j++) {
			subset[i - 1]->data[len1 + j + 1] = subset[i]->data[j];
		}
		subset[i]->data_count = 0;
		//���ұ߽ڵ�ĺ�����Ϣ�ӵ���߽ڵ�ȥ
		for (int j = 0; j < childsize2; j++) {
			subset[i - 1]->subset[childsize1 + j] = subset[i]->subset[j];
			subset[i]->subset[j] = nullptr;
		}
		subset[i]->child_count = 0;
		//�ƶ�this��subset
		for (int j = i; j < child_count; j++) {
			subset[j] = subset[j + 1];
		}
		subset[i - 1]->child_count += childsize2;
		subset[i - 1]->data_count += (len2+1);
		--child_count;

		//��Ҫ�ͷŽڵ�,ʹ������ָ��������
		delete subset[i];
	}
	//�ڵ�������ߣ����������һ��Ԫ��
	else if (i == 0 && subset[i]->data_count == MINIMUM - 1) {
		T first=data[0];
		int j = 0;
		for (j = 0; j < data_count - 1; j++) {
			data[j] = data[j + 1];
		}
		--data_count;
		int len1 = subset[0]->data_count;
		int len2 = subset[1]->data_count;
		int childsize1 = subset[0]->child_count;
		int childsize2 = subset[1]->child_count;

		
		subset[0]->data[len1] = first;
		for (j = 0; j < len2; j++) {
			subset[0]->data[len1 + 1 + j] = subset[1]->data[j];
		}
		for (j = 0; j < childsize2; j++) {
			subset[0]->subset[childsize1 + j] = subset[1]->subset[j];
		}
		for (j = 1; j < child_count-1; j++) {
			subset[j] = subset[j + 1];
		}
		--child_count;

		subset[0]->data_count += (len2 + 1);
		subset[0]->child_count += childsize2;

		delete subset[1];
	}
	else {}
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

		//j��ʾ������һ���������һ��
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

			left->data_count = MINIMUM;
			right->data_count = MINIMUM;
			if (this->child_count == 0) {
				left->child_count = right->child_count = 0;
			}
			else {
				left->child_count = MINIMUM + 1;
				right->child_count = MINIMUM + 1;

			}
			//���ָ��ڵ㲻�䣬�޸�������
			this->data[0] = data[MINIMUM];
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
	SetMe<T>* node = new SetMe<T>();
	//�����ڵ��Ԫ�صĺ��Ƽ������ӽڵ��м��Ԫ�ص����ʵ�λ��
	for (int j = data_count - i; j > 0; j--) {
		data[j] = data[j-1];
	}
	data[i] = subset[i]->data[MINIMUM];
	//���ӽڵ�ֳ����ݣ�nodeΪ�ұߵ�һ����
	++data_count;


	//��һ��ֱ�Ӽ�ָ�룬���ʹ��󣡣�
	//node.data = subset[i]->data + MINIMUM;

	//�����µ�ֵ���ӽڵ����Ϣ
	for (int j = 0; j < MINIMUM ; j++) {
		node->data[j] = subset[i]->data[MINIMUM + j+1];
		node->subset[j] = subset[i]->subset[MINIMUM + j];
	}
	if (subset[i]->child_count == 0) {
		subset[i]->data_count = MINIMUM;
		subset[i]->child_count = 0;
		node->child_count = 0;
		node->data_count=MINIMUM;
	}
	else
	{
		node->child_count = MINIMUM + 1;
		node->data_count = MINIMUM;
		subset[i]->data_count = MINIMUM;
		subset[i]->child_count = MINIMUM + 1;
	}

	//���µ��ҽڵ�node���£���Ҫ�Ѳ���subset����
	for (int j = child_count-1; j > i; j--) {
		subset[j+1] = subset[j];
		
	}
	subset[i+1] = node;
	++child_count;

}
template<typename T>
bool SetMe<T>::loose_insert(const T& entry,std::vector<SetMe<T>*> &vec,std::vector<int>& index)
{
	std::size_t i;
	//��õ�һ����С�ڲ���ֵ��Ԫ�ص�����
	for (i = 0; entry >data[i] && i < data_count; i++) {};
	//Ӧ�ü������i��data�Ĺ�ϵ�����i��data_count��Ⱦ�˵������ĩβ
	if (entry == data[i] && i <data_count) { return false; }
	else if (child_count == 0) {
		//û���ӽڵ㣬�ݹ�ĳ���
		//��Ҫ������ֵ����
		for (int j = data_count; j > i; j--) {
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

template<typename T>
void SetMe<T>::Show()
{
	std::queue<SetMe<T>*> que;
	
	que.push(this);
	que.push(nullptr);
	while (!que.empty()) {
		int i = 0;

		//SetMe<T>* temp = que.front();
		auto temp = que.front();
		if (temp == nullptr) {
			std::cout << std::endl;
		}
		else{
			for (i = 0; i < temp->data_count; i++) {
				std::cout << temp->data[i] << std::ends;
			}
			for (i = 0; i < temp->child_count; i++) {
				que.push(temp->subset[i]);
				if (i == temp->child_count-1) {
					que.push(nullptr);
				}
			}
		}
		que.pop();
		//que.front()!= nullptr&&!que.empty()
		//˳�����Ҫ		
		if (!que.empty() && que.front() != nullptr) {
			std::cout << "--|||--";
		}
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