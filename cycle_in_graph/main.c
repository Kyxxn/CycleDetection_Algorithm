#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX 1001

typedef struct node{
    int vertex;
    struct node* link;
}Vertex;

typedef struct graph{
    Vertex* list[MAX]; // 인접리스트
    int route[MAX]; // 나를 호출한 부모 정점의 키 값 보유 [사이클 경로]
    bool visited[MAX]; // 정점 방문 여부를 기록하는 배열
    bool finished[MAX]; // 해당 정점을 대상으로 호출한 함수 종료 여부 [사이클 판별]
    bool back[MAX]; // 다른 사이클의 역방향 간선인 지 확인 [역방향 간선 중첩현상 해결]
}Graph;

typedef struct stack{
    int stack_arr[MAX];
    int top;
}Stack;

void stack_Init(Stack* s){
    s->top = -1;
}

bool stack_Empty(Stack* s){
    return s->top == -1;
}

void stack_Push(Stack* s, int data){
    s->stack_arr[++(s->top)] = data;
}

int stack_pop(Stack * s){
    return s->stack_arr[(s->top)--];
}

Graph* createGraph(int vertex){
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    for (int i = 0; i < vertex; i++) {
        graph->visited[i] = graph->finished[i] = graph->back[i] = false;
        graph->list[i] = NULL;
        graph->route[i] = -1;
    }
    
    return graph;
}

Vertex* createNode(int V){
    Vertex* node = (Vertex*)malloc(sizeof(Vertex));
    node->vertex = V;
    node->link = NULL;
    
    return node;
}

void addEdge(Graph* graph, int start, int end){
    Vertex* node = createNode(end);
    node->link = graph->list[start];
    graph->list[start] = node;
}

void printCycle(Stack* s, int vertex, int cycleCount) {
    printf("%d번째 사이클 경로 : ", cycleCount);
    while (!stack_Empty(s)) {
        printf("%d -> ", stack_pop(s));
    }
    printf("%d\n", vertex);
}

bool cycle_DFS(Graph* graph, int V, int parent, int* cycleCount){
    // 방문하지 않은 정점 혹은 다른 사이클의 역방향 간선인 경우
    if(!graph->visited[V] || graph->back[V]){
        graph->visited[V] = true;
        graph->finished[V] = true;
        graph->route[V] = parent;
        
        // 인접한 정점 방문
        Vertex* temp = graph->list[V];
        while(temp != NULL){
            // 1. 방문하지 않은 정점이고, DFS를 실행했을 때 참이면 사이클 개수 1 증가
            if(!graph->visited[temp->vertex] && cycle_DFS(graph, temp->vertex, V, cycleCount)){
                (*cycleCount)++;
            }
            // 2. 방문한 정점이고 나를 호출한 부모 정점일 때, 사이클 발견
            else if(graph->finished[temp->vertex]){
                // 스택에 넣어 순방향 출력
                Stack s;
                stack_Init(&s);
                
                int cur = V;
                while(cur != temp->vertex){
                    stack_Push(&s, cur);
                    cur = graph->route[cur];
                }
                stack_Push(&s, temp->vertex);
                printCycle(&s, temp->vertex, *cycleCount + 1);
                
                printf("역방향 간선 : %d -> %d\n\n", V, temp->vertex);
                // 사이클에 대해 종료하고 해당 정점이 역방향 간선임을 체크 후 참을 반환
                graph->finished[V] = false;
                graph->back[V] = true;
                return true;
            }
            // 이미 방문한 정점이고, 다른 사이클의 역방향 간선으로 존재하는 경우
            else if(!graph->finished[temp->vertex] && graph->visited[temp->vertex]){
                if(cycle_DFS(graph, temp->vertex, V, cycleCount)) (*cycleCount)++;
            }
            temp = temp->link;
        }
    }
    // 인접리스트를 모두 방문해도 사이클을 찾지 못하면 거짓을 반환
    graph->finished[V] = false;
    return false;
}

int main(void){
    int vertex = 6;
    Graph* graph = createGraph(vertex);
    
    // 정점 개수 = 6, 간선 개수 = 8, 그래프 내의 사이클 총 개수 = 3
    // 1번 사이클 : 2 -> 4 -> 5 -> 2
    // 2번 사이클 : 2 -> 3 -> 5 -> 2
    // 3번 사이클 : 0 -> 2 -> 3 -> 0
    addEdge(graph, 0, 1);
    addEdge(graph, 0, 2);
    
    addEdge(graph, 2, 3);
    addEdge(graph, 2, 4);
    
    addEdge(graph, 3, 0);
    addEdge(graph, 3, 5);
    
    addEdge(graph, 4, 5);
    addEdge(graph, 5, 2);
    
    int cycleCount = 0;
    // 모든 정점에 대해 cycle_DFS 실행
    for(int i=0; i<vertex; i++){
        if(!graph->visited[i] && cycle_DFS(graph, i, -1, &cycleCount)){
            cycleCount++;
        }
    }
    printf("그래프 내에 사이클 총 개수 : %d개 입니다.\n", cycleCount);
    
    free(graph);
    return 0;
}

