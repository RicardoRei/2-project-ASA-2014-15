#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


typedef struct node* link;
typedef struct Vertex* item; /* para facilitar a leitura de ponteiros para a estrutura vertex e node*/

/*
*@brief Vertex
* 		estrutura que vai guardar informacao relatica a um vertice.
*/
struct Vertex
{
	int active;
	int mark;
	int cycle;
	int id;
	int d;
};

/*
*@brief struct node
* 		estrutura que vai auxiliar na construcao da lista de adjacencias
*/
struct node
{
	item _vertex;
	link next;
	int cost;
};

/*
*@brief Queue
* 		fila do tipo FIFO.
*/
typedef struct
{
	link head;
	link tail;
} Queue;

/*
*@brief struct Graph:
*		estrutura que contem uma lista de adjacencias, o numero de vertices, arestas, a fonte um vector com as arestas
*		e um vector com os vertices.
*/
typedef struct
{
	link* adjacenciesList;
	int verticesNumber;
	
	item _source;
	item* _vertices;

}Graph;

item createVertex(int i);
link createNode(item i, int cost);
link insertLinkedList(item i, link head, int cost);
link dequeue(Queue* queue);
void enqueue(Queue *queue, item i);

Graph builtGraph();
int SPFAalgorithm(Graph *graph);
void markVertices(Graph *graph, item vertex);
void outPut(Graph graph);

int main()
{
	Graph graph = builtGraph();
	SPFAalgorithm(&graph);
	outPut(graph);

	return EXIT_SUCCESS;
}

/*
*@brief builtGraph;
*		Le do standard input o numero de vertices e arestas, o numero do vertice que representa a sede e as transicoes/arestas.
*		Constroi o grafo com essa informacao.
*		
*/
Graph builtGraph()
{
	int vertices, edges/*numero de custos*/,
		source, vertexOne, vertexTwo, i, cost;

	Graph graph;

	scanf("%d %d", &vertices, &edges);
	graph.adjacenciesList = (link*) malloc(sizeof(link)*vertices);
	graph.verticesNumber = vertices;
	graph._vertices = (item*) malloc(sizeof(item)*vertices);

	for ( i = 0; i < vertices; i++)
	{
		graph.adjacenciesList[i] = NULL;
		graph._vertices[i] = createVertex(i);
	}

	scanf("%d", &source);
	graph._source = graph._vertices[source-1];
	graph._source->d = 0;

	for (i = 0; i < edges; i++)
	{
		scanf("%d %d %d", &vertexOne, &vertexTwo, &cost);
		graph.adjacenciesList[vertexOne-1] = insertLinkedList(graph._vertices[vertexTwo-1], graph.adjacenciesList[vertexOne-1], cost);
	}
	return graph;
}

/*
*@brief SPFAalgorithm;
*		Recebe o um grafo e executa o algoritmo shortest-path-faster-algorithm, um algortimo derivado do bellman-ford com uma
*		melhor complexidade em casos medios, e complexidade igual no pior caso.
*/
int SPFAalgorithm(Graph *graph)
{
	int j;
	link U, current;

	Queue* queue = (Queue*) malloc(sizeof(Queue)); /*inicializacao da fila*/
	queue->head = NULL;
	queue->tail = NULL;

	enqueue(queue, graph->_source);

	while (queue->head != NULL)
	{
		U = dequeue(queue);
		current = graph->adjacenciesList[U->_vertex->id];
		while (current != NULL)	
		{
			if (current->_vertex->d > U->_vertex->d + current->cost &&
				U->_vertex->d != INT_MAX)
			{
				current->_vertex->d = U->_vertex->d + current->cost;
				if (current->_vertex->active == 0 && current->_vertex->mark < graph->verticesNumber)
					enqueue(queue, current->_vertex);
			}
			current = current->next;
		}
		free(U);
	}

	for (j = 0; j < graph->verticesNumber; j++)
	{
		if (graph->_vertices[j]->mark == graph->verticesNumber)
			markVertices(graph, graph->_vertices[j]);		
	}

	return EXIT_SUCCESS;
}

/*
*@brief markVertices
*		funcao que percorre o grafo marcando os vertices que pertencem ao ciclo detectado pelo SPFAalgorithm.
*/
void markVertices(Graph *graph, item vertex)
{
	link current = graph->adjacenciesList[vertex->id];

	while (current != NULL)
	{
		if (current->_vertex->cycle == 0)
		{
			current->_vertex->cycle++;
			markVertices(graph, current->_vertex);
			current = current->next;
		}
		else
			current = current->next;
	}
}

/*
*@brief outPut
*		funcao que nos da com base em toda a estrutura o output esperado.
*/
void outPut(Graph graph)
{
	int i;
	for(i = 0; i < graph.verticesNumber; i++)
	{
		if (graph._vertices[i]->cycle == 1)
			printf("I\n");

		else if (graph._vertices[i]->d == INT_MAX)
			printf("U\n");

		else
			printf("%d\n",graph._vertices[i]->d);
	}
}
/*
*@brief dequeue:
*		retira da fila o primeiro elemento.
*/
link dequeue(Queue* queue)
{
	link aux = queue->head;
	aux->_vertex->active = 0;

	if (queue->head == NULL && queue->tail == NULL)
	{
		return NULL;
	}
	else if (queue->head->next == NULL && queue->tail->next == NULL)
	{
		queue->head = queue->head->next;
		queue->tail = queue->head;
	}
	else
		queue->head = queue->head->next;
	return aux;
}

/*
*@brief enqueue:
*		insere na final da fila um elemento.
*/
void enqueue(Queue *queue, item i)
{
	link new = createNode(i, 0);
	i->active = 1;
	i->mark++;
	if( queue->head == NULL && queue->tail == NULL)
	{
		queue->head = new;
		queue->tail = new;
	}
	else
	{
		queue->tail->next = new;
		queue->tail = new;
	}
}

/*
*@brief createVertex;
*		Funcao que cria um vertice inicialmente com d (distancia a sede) a INT-MAX(infinito), a mark a 0 ()
*/
item createVertex(int i)
{
	item new = (item)malloc(sizeof(struct Vertex));
	new->id = i;
	new->d = INT_MAX;
	new->mark = 0;
	new->active = 0;
	new->cycle = 0;

	return new;
}

/*
*@brief createNode:
*		cria um no atraves de um vertice.
*/
link createNode(item i, int cost)
{
	link new = (link) malloc(sizeof(struct node));
	new->_vertex = i;
	new->next = NULL;
	new->cost = cost;
	return new;
}

/*
*@brief insertLinkedList:
*		insere um elemento no inicio da lista.
*/
link insertLinkedList(item i, link head, int cost)
{
	link new = createNode(i, cost);
	if (head == NULL)
		head = new;
	else
	{
		new->next = head;
		head = new;
	}
	return head;
}
