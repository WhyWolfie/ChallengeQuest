#pragma once


//todok FSM으로 AI를 정의하고 있는데 AI자체가 외부로 빠져있다고 보면 된다.
// 그런데 이런 상황에서 actor와 brain을 분리하는 것이 큰 의미가 있을까?
// FSM을 사용하는 코드 정도가 전부일텐데 굳이 actor와 별도의 클래스로 분리해서
// 통신비용을 감수하느니 그냥 actor에서 직접 FSM을 핸들링하고 brain은 없는 편이 나을지도 모른다
// 
// 그런 의미에서 brain은 버릴 생각을 하고 actor 내부에서 직접 FSM핸들링 코드를 짜도록 하자
class ZBrainWithFSM
{

};