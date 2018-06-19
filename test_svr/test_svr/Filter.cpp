#include "Filter.h"
#include "const.h"

static double m_Array[200000];

void LogTime()
{
	LARGE_INTEGER lv;

	// ��ȡÿ�����CPU Performance Tick
	QueryPerformanceFrequency( &lv );

	// ת��Ϊÿ��Tick������
	double secondsPerTick = 1.0 / lv.QuadPart;

	// ��ȡCPU���е����ڵ�Tick��
	QueryPerformanceCounter( &lv );

	// ����CPU���е����ڵ�ʱ��
	// ��GetTickCount��timeGetTime���Ӿ�ȷ
	double timeElapsedTotal = secondsPerTick * lv.QuadPart;

	//cout.precision( 6 );
	//cout << fixed << showpoint << timeElapsedTotal << endl;

	g_logger.TraceWarning("time now is %.6f",timeElapsedTotal);

}
//-------------����
void Swap(double A[], int i, int j)
{
	double temp = A[i];
	A[i] = A[j];
	A[j] = temp;
}

int Partition(double A[], int left, int right)  // ���ֺ���
{
	//g_logger.TraceInfo("Partition,%d-%d",left,right);
	double pivot = A[right];               // ����ÿ�ζ�ѡ�����һ��Ԫ����Ϊ��׼
	int tail = left - 1;                // tailΪС�ڻ�׼�����������һ��Ԫ�ص�����
	for (int i = left; i < right; i++)  // ������׼���������Ԫ��
	{
		if (A[i] <= pivot)              // ��С�ڵ��ڻ�׼��Ԫ�طŵ�ǰһ��������ĩβ
		{
			Swap(A, ++tail, i);
		}
	}
	Swap(A, tail + 1, right);           // ���ѻ�׼�ŵ�ǰһ��������ĺ�ߣ�ʣ�µ���������Ǵ��ڻ�׼��������
	// �ò������п��ܰѺ���Ԫ�ص��ȶ��Դ��ң����Կ��������ǲ��ȶ��������㷨
	return tail + 1;                    // ���ػ�׼������
}

void QuickSort(double A[], int left, int right)
{
	//g_logger.TraceInfo("QuickSort,%d-%d",left,right);
	if (left >= right)
		return;
	int pivot_index = Partition(A, left, right); // ��׼������
	QuickSort(A, left, pivot_index - 1);
	QuickSort(A, pivot_index + 1, right);
}
//-------------����

void BubbleSort(double A[], double n)
{
	for (int j = 0; j < n - 1; j++)         // ÿ�����Ԫ�ؾ�������һ��"��"����������
	{
		for (int i = 0; i < n - 1 - j; i++) // ���αȽ����ڵ�����Ԫ��,ʹ�ϴ���Ǹ������
		{
			if (A[i] > A[i + 1])            // ��������ĳ�A[i] >= A[i + 1],���Ϊ���ȶ��������㷨
			{
				Swap(A, i, i + 1);
			}
		}
	}
}

//-------������----begin
void Heapify(double A[], int i, int size)  // ��A[i]���½��жѵ���
{
	int left_child = 2 * i + 1;         // ��������
	int right_child = 2 * i + 2;        // �Һ�������
	int max = i;                        // ѡ����ǰ����������Һ�������֮�е����ֵ
	if (left_child < size && A[left_child] > A[max])
		max = left_child;
	if (right_child < size && A[right_child] > A[max])
		max = right_child;
	if (max != i)
	{
		Swap(A, i, max);                // �ѵ�ǰ�����������(ֱ��)�ӽڵ���н���
		Heapify(A, max, size);          // �ݹ���ã������ӵ�ǰ������½��жѵ���
	}
}

int BuildHeap(double A[], int n)           // ���ѣ�ʱ�临�Ӷ�O(n)
{
	int heap_size = n;
	for (int i = heap_size / 2 - 1; i >= 0; i--) // ��ÿһ����Ҷ��㿪ʼ���½��жѵ���
		Heapify(A, i, heap_size);
	return heap_size;
}

void HeapSort(double A[], int n)
{
	int heap_size = BuildHeap(A, n);    // ����һ������
	while (heap_size > 1)// �� ������ Ԫ�ظ�������1��δ�������
	{   // ���Ѷ�Ԫ����ѵ����һ��Ԫ�ػ��������Ӷ���ȥ�����һ��Ԫ��
		// �˴������������п��ܰѺ���Ԫ�ص��ȶ��Դ��ң����Զ������ǲ��ȶ��������㷨
		Swap(A, 0, --heap_size);
		Heapify(A, 0, heap_size);     // ���µĶѶ�Ԫ�ؿ�ʼ���½��жѵ�����ʱ�临�Ӷ�O(logn)
	}
}
//-------������----end

Filter::Filter(UINT nSize)
	:m_dSum(0.0)
	,m_nCount(0)
	,m_pArrayDouble(NULL)
	,m_bInitialed(false)
{
	m_nBufSize = nSize;
	g_logger.TraceInfo("Filter::Filter -in");

	memset(m_Array,0,sizeof(double)*100000);
}

Filter::~Filter(void)
{
	g_logger.TraceInfo("Filter::~Filter");
	if (NULL != m_pArrayDouble)
	{
		delete[] m_pArrayDouble;
		m_pArrayDouble = NULL;
	}
}



void Filter::AddData(double dData)
{
	m_dSum += dData;
	m_nCount++;
}

void Filter::AddData1(double dData)
{
	try
	{
		if (!m_bInitialed)
		{
			memset(m_Array,0,sizeof(double)*100000);
			//g_logger.TraceInfo("Filter::AddData1 - init1");
			//Initial();
			//g_logger.TraceInfo("Filter::AddData1 - init2-%.3f,%d,",dData,m_nCount);
		}
		m_Array[m_nCount] = dData;
		//*(m_pArrayDouble+m_nCount) = dData;
		m_nCount++;
	}
	catch (exception &e)
	{
		DWORD dw = GetLastError();
		g_logger.TraceError("Filter::AddData:%d-%s",dw,e.what());
	}

}

void Filter::ResetMid()
{
	m_nCount = 0;
	m_bInitialed = false;
	//g_logger.TraceInfo("Filter::ResetMid");
}

void Filter::Initial()
{

	try{
		g_logger.TraceInfo("Filter::Initial1");
		if (NULL !=m_pArrayDouble)
		{
			g_logger.TraceInfo("Filter::Initial2");
			delete[] m_pArrayDouble;
			m_pArrayDouble = NULL;
		}
		g_logger.TraceInfo("Filter::Initial3");
		m_pArrayDouble = new double[m_nBufSize];
		g_logger.TraceInfo("Filter::Initial4");
		memset(m_pArrayDouble,0,sizeof(double)*m_nBufSize);
		g_logger.TraceInfo("Filter::Initial5");
		m_bInitialed = true;
		m_nCount = 0;
		g_logger.TraceInfo("Filter::Initial6");

	}
	catch (exception &e)
	{
		DWORD dw = GetLastError();
		g_logger.TraceError("Filter::GetMidValue:%d-%s",dw,e.what());
	}
}

double Filter::GetMidValue()
{
	try{
		g_logger.TraceInfo("Filter::GetMidValue,sum point count is %d.",m_nCount);
		//double* ptmp = new double[m_nBufSize];

		//double tmp[30000] = {0};
		////if ()
		////{
		////}
		//if (m_pArrayDouble == NULL)
		//{
		//}
		//g_logger.TraceInfo("Filter::GetMidValue2");
		//memcpy(tmp,m_pArrayDouble,sizeof(double)*m_nBufSize);
		//g_logger.TraceInfo("Filter::GetMidValue3");

		//LogTime();
		//g_logger.TraceInfo("Filter::GetMidValue5");
		//QuickSort(tmp, 0, m_nCount-1);//DAQCONTROLER::sectionLengthW - 1
		//g_logger.TraceInfo("Filter::GetMidValue3");
		//LogTime();
		////delete[] ptmp;
		//g_logger.TraceInfo("Filter::GetMidValue6");
		////ptmp = NULL;

		//QuickSort(m_pArrayDouble, 0, m_nCount-1);//DAQCONTROLER::sectionLengthW - 1
		//QuickSort(m_Array, 0, 1000-1);//data is big, calc failed.
		//BubbleSort(m_Array, m_nCount);//
		HeapSort(m_Array, m_nCount);//
		for (int i=0;i<m_nCount;)
		{
			for (int j=0;j<20;j++)
			{
				i+=20;
				g_logger.TraceError("%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t%.3f\t",
					m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],
					m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i],m_Array[i]);
				//g_logger.TraceError("%.3f",m_Array[i]);
			}
		}
		//m_bInitialed = false;

		//g_logger.TraceInfo("Filter::GetMidValue %d",m_nCount);

		double dret = m_Array[(m_nCount)/2];
		return dret;

		//return *( m_pArrayDouble + (m_nCount)/2 );//DAQCONTROLER::sectionLengthW

	}
	catch (exception &e)
	{
		DWORD dw = GetLastError();
		g_logger.TraceError("Filter::GetMidValue:%d-%s",dw,e.what());
	}

}

double Filter::GetMaxValue()
{
	try{

		HeapSort(m_Array, m_nCount);



		//m_bInitialed = false;

		//g_logger.TraceInfo("Filter::GetMidValue %d",m_nCount);

		double dret = m_Array[(m_nCount-1)];
		return dret;

		//return *( m_pArrayDouble + (m_nCount)/2 );//DAQCONTROLER::sectionLengthW

	}
	catch (exception &e)
	{
		DWORD dw = GetLastError();
		g_logger.TraceError("Filter::GetMidValue:%d-%s",dw,e.what());
	}

}


double Filter::GetMeanData()
{
	if (0 != m_nCount)
	{
		return m_dSum / m_nCount;
	}
	else
	{
		return 0;
	}
}