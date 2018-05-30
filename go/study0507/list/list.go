package list

type Element struct {
	prev, next *Element
	list *List
	Value interface{}
}

func (e *Element) Next() *Element {
	if p:=e.next; e.list != nil && p != &e.list.root {
		return p
	}
	return nil
}

func (e *Element) Prev() *Element {
	if p:=e.prev; e.list != nil && p != &e.list.root {
		return p
	}
	return nil
}

type List struct {
	len int
	root Element
}

func (l *List) Init() *List {
	l.root.next = &l.root
	l.root.prev = &l.root
	l.len = 0
	return l
}

func New() *List {
	return new(List).Init()
}

func (l *List) Back() *Element {
	if l.len == 0 {
		return nil
	}
	return l.root.prev
}

func (l *List) Len() int {
	return l.len
}

func (l *List) Front() *Element {
	if l.len == 0 {
		return nil
	}
	return l.root.next
}

func (l *List) insert (e, at *Element) *Element{
	n := at.next
	at.next = e
	e.prev = at
	e.next = n
	n.prev = e
	e.list = l
	l.len++
	return e
}
func (l *List) insertValue(v interface{}, at *Element) *Element {
	return l.insert(&Element{Value:v}, at)
}
func (l *List) PushBack(v interface{}) *Element{
	return l.insertValue(v, l.root.prev)
}
func (l *List) PushPrev(v interface{}) *Element {
	return l.insertValue(v, &l.root)
}

func (l *List) remove (e *Element) *Element{
	e.prev.next = e.next
	e.next.prev = e.prev
	e.next = nil
	e.prev = nil
	e.list = nil
	l.len--
	return e
}

func (l *List) Remove(e *Element) interface{} {
	if e.list == l {
		l.remove(e)
	}
	return e.Value
}

