/*
 * ============================================================
 *  SISTEMA DE GESTIÓN - PAPELERÍA "POPS"
 *  Proyecto Final - Algoritmos y Estructura de Datos
 * ============================================================
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <stack>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

// ============================================================
//  UNION
// ============================================================
union EstadoProducto {
    int codigoEstado;
    char etiqueta[12];
};

// ============================================================
//  ESTRUCTURAS
// ============================================================
struct Fecha {
    int dia, mes, anio;
};

struct Producto {
    int id;
    char nombre[40];
    char categoria[30];
    double precio;
    int stock;
    int stockMinimo;
    Fecha caducidad;
    int idProveedor;
    EstadoProducto estado;
};

struct Proveedor {
    int id;
    char empresa[50];
    char telefono[20];
    char correo[40];
};

struct Cliente {
    int id;
    char nombre[40];
    char telefono[20];
};

struct Empleado {
    int id;
    char nombre[40];
    char puesto[30];
    double salario;
};

struct Devolucion {
    int idVenta;
    int idProducto;
    int cantidad;
    char motivo[50];
    Fecha fecha;
};

struct ItemVenta {
    int idProducto;
    int cantidad;
};

struct Venta {
    int id;
    int idCliente;
    int idEmpleado;
    Fecha fecha;
    double total;
    vector<ItemVenta> items;
};

// ============================================================
// Utilidades de cadenas C
// ============================================================
void copiarCadena(char* dest, const char* src, size_t maxLen) {
    strncpy(dest, src, maxLen - 1);
    dest[maxLen - 1] = '\0';
}

int compararCadena(const char* a, const char* b) { return strcmp(a, b); }
int longitudCadena(const char* s) { return (int)strlen(s); }

void concatenarCadena(char* dest, const char* src, size_t maxLen) {
    size_t actual = strlen(dest);
    if (actual < maxLen - 1)
        strncat(dest, src, maxLen - actual - 1);
}

// ============================================================
// LISTA SIMPLE
// ============================================================
struct NodoSimple {
    Devolucion dato;
    NodoSimple* sig;

    NodoSimple(const Devolucion& d) : sig(NULL) {
        dato.idVenta    = d.idVenta;
        dato.idProducto = d.idProducto;
        dato.cantidad   = d.cantidad;
        copiarCadena(dato.motivo, d.motivo, sizeof(dato.motivo));
        dato.fecha = d.fecha;
    }
};

class ListaSimple {
    NodoSimple* cabeza;
public:
    ListaSimple() : cabeza(NULL) {}
    ListaSimple(const ListaSimple&);
    ListaSimple& operator=(const ListaSimple&);

    ~ListaSimple() {
        while (cabeza) {
            NodoSimple* t = cabeza;
            cabeza = cabeza->sig;
            delete t;
        }
    }

    void agregar(const Devolucion& d) {
        NodoSimple* n = new NodoSimple(d);
        n->sig = cabeza;
        cabeza = n;
    }

    void mostrar() const {
        cout << "\n--- Devoluciones registradas ---\n";
        const NodoSimple* aux = cabeza;
        if (!aux) { cout << "  (ninguna)\n"; return; }
        while (aux) {
            cout << "  Venta#" << aux->dato.idVenta
                 << " Prod#" << aux->dato.idProducto
                 << " x" << aux->dato.cantidad
                 << " (" << aux->dato.motivo << ")\n";
            aux = aux->sig;
        }
    }
};

// ============================================================
// LISTA DOBLE
// ============================================================
struct NodoDoble {
    Venta dato;
    NodoDoble* sig;
    NodoDoble* ant;

    NodoDoble(const Venta& v) : dato(v), sig(NULL), ant(NULL) {}
};

class ListaDoble {
    NodoDoble* cabeza;
    NodoDoble* cola;
public:
    ListaDoble() : cabeza(NULL), cola(NULL) {}
    ListaDoble(const ListaDoble&);
    ListaDoble& operator=(const ListaDoble&);

    ~ListaDoble() {
        while (cabeza) {
            NodoDoble* t = cabeza;
            cabeza = cabeza->sig;
            delete t;
        }
    }

    void agregar(const Venta& v) {
        NodoDoble* n = new NodoDoble(v);
        if (!cabeza) { cabeza = cola = n; return; }
        cola->sig = n;
        n->ant = cola;
        cola = n;
    }

    void mostrar() const {
        cout << "\n--- Historial de Ventas ---\n";
        const NodoDoble* aux = cabeza;
        if (!aux) { cout << "  (ninguna)\n"; return; }
        while (aux) {
            cout << "  Venta#" << aux->dato.id
                 << " Cli#" << aux->dato.idCliente
                 << " Emp#" << aux->dato.idEmpleado
                 << " Total:$" << fixed << setprecision(2) << aux->dato.total
                 << " Fecha:" << aux->dato.fecha.dia << "/"
                 << aux->dato.fecha.mes << "/" << aux->dato.fecha.anio << "\n";
            aux = aux->sig;
        }
    }
};

// ============================================================
//  MAPA PROPIO
// ============================================================
template <typename K, typename V>
class MapaPropio {
    struct Par { K clave; V valor; Par* sig; };
    static const int TAM = 101;
    Par* tabla[TAM];
    int hash(K clave) const { return ((long)clave) % TAM; }
public:
    MapaPropio() { for (int i = 0; i < TAM; i++) tabla[i] = NULL; }
    MapaPropio(const MapaPropio&);
    MapaPropio& operator=(const MapaPropio&);

    ~MapaPropio() {
        for (int i = 0; i < TAM; i++) {
            Par* p = tabla[i];
            while (p) { Par* t = p; p = p->sig; delete t; }
        }
    }

    void insertar(K clave, V valor) {
        int h = hash(clave);
        Par* p = tabla[h];
        while (p) {
            if (p->clave == clave) { p->valor = valor; return; }
            p = p->sig;
        }
        Par* n = new Par;
        n->clave = clave;
        n->valor = valor;
        n->sig = tabla[h];
        tabla[h] = n;
    }

    bool obtener(K clave, V& out) const {
        int h = hash(clave);
        Par* p = tabla[h];
        while (p) {
            if (p->clave == clave) { out = p->valor; return true; }
            p = p->sig;
        }
        return false;
    }

    void mostrar() const {
        for (int i = 0; i < TAM; i++) {
            Par* p = tabla[i];
            while (p) {
                cout << "  [" << p->clave << "] -> " << p->valor << "\n";
                p = p->sig;
            }
        }
    }
};

// ============================================================
//  PILA
// ============================================================
class PilaDevoluciones {
    stack<Devolucion> p;
public:
    void push(const Devolucion& d) { p.push(d); }
    bool pop(Devolucion& d) {
        if (p.empty()) return false;
        d = p.top();
        p.pop();
        return true;
    }
    bool vacia() const { return p.empty(); }
};

// ============================================================
// COLA
// ============================================================
class ColaClientes {
    queue<Cliente> c;
public:
    void encolar(const Cliente& x) { c.push(x); }
    bool desencolar(Cliente& x) {
        if (c.empty()) return false;
        x = c.front();
        c.pop();
        return true;
    }
    bool vacia() const { return c.empty(); }
    size_t tam() const { return c.size(); }
};

// ============================================================
//  COLA DE PRIORIDAD
// ============================================================
struct PedidoProveedor {
    int idProducto;
    char nombreProducto[40];
    char empresaProveedor[50];
    int cantidad;
    int prioridad;
    bool operator<(const PedidoProveedor& o) const {
        return prioridad > o.prioridad;
    }
};

class ColaPrioridadPedidos {
    priority_queue<PedidoProveedor> pq;
public:
    void agregar(const PedidoProveedor& p) { pq.push(p); }
    bool sacar(PedidoProveedor& p) {
        if (pq.empty()) return false;
        p = pq.top();
        pq.pop();
        return true;
    }
    bool vacia() const { return pq.empty(); }

    vector<PedidoProveedor> aVector() const {
        ColaPrioridadPedidos copia = *this;
        vector<PedidoProveedor> v;
        PedidoProveedor p;
        while (copia.sacar(p)) v.push_back(p);
        return v;
    }
};

// ============================================================
// ÁRBOL BINARIO DE BÚSQUEDA
// ============================================================
struct NodoArbol {
    Producto dato;
    NodoArbol* izq;
    NodoArbol* der;
    NodoArbol(const Producto& p) : dato(p), izq(NULL), der(NULL) {}
};

class ArbolProductos {
public:
    NodoArbol* raiz;
    ArbolProductos() : raiz(NULL) {}
    ArbolProductos(const ArbolProductos&);
    ArbolProductos& operator=(const ArbolProductos&);

    ~ArbolProductos() { destruir(raiz); }

    void destruir(NodoArbol* n) {
        if (!n) return;
        destruir(n->izq);
        destruir(n->der);
        delete n;
    }

    NodoArbol* insertarRec(NodoArbol* n, const Producto& p) {
        if (!n) return new NodoArbol(p);
        if (p.id < n->dato.id)      n->izq = insertarRec(n->izq, p);
        else if (p.id > n->dato.id) n->der = insertarRec(n->der, p);
        return n;
    }
    void insertar(const Producto& p) { raiz = insertarRec(raiz, p); }

    NodoArbol* buscarRec(NodoArbol* n, int id) {
        if (!n || n->dato.id == id) return n;
        if (id < n->dato.id) return buscarRec(n->izq, id);
        return buscarRec(n->der, id);
    }
    Producto* buscar(int id) {
        NodoArbol* n = buscarRec(raiz, id);
        return n ? &n->dato : NULL;
    }

    void inOrden(NodoArbol* n, vector<Producto>& out) {
        if (!n) return;
        inOrden(n->izq, out);
        out.push_back(n->dato);
        inOrden(n->der, out);
    }
    vector<Producto> listar() { vector<Producto> v; inOrden(raiz, v); return v; }
};

// ============================================================
// GRAFO DIRIGIDO CON PESO
// ============================================================
struct AristaGrafo {
    int destino;
    double peso;
    AristaGrafo* sig;
};

class GrafoProveedores {
    map<int, AristaGrafo*> adj;
public:
    GrafoProveedores() {}
    GrafoProveedores(const GrafoProveedores&);
    GrafoProveedores& operator=(const GrafoProveedores&);

    ~GrafoProveedores() {
        for (map<int, AristaGrafo*>::iterator it = adj.begin(); it != adj.end(); ++it) {
            AristaGrafo* a = it->second;
            while (a) { AristaGrafo* t = a; a = a->sig; delete t; }
        }
    }

    void agregar(int idProv, int idProd, double costo) {
        AristaGrafo* n = new AristaGrafo;
        n->destino = idProd;
        n->peso = costo;
        n->sig = adj[idProv];
        adj[idProv] = n;
    }

    void mostrar() const {
        cout << "\n--- Grafo Proveedor -> Producto (costo) ---\n";
        for (map<int, AristaGrafo*>::const_iterator it = adj.begin(); it != adj.end(); ++it) {
            cout << "Proveedor #" << it->first << ":\n";
            AristaGrafo* a = it->second;
            while (a) {
                cout << "   -> Producto #" << a->destino
                     << " (costo $" << fixed << setprecision(2) << a->peso << ")\n";
                a = a->sig;
            }
        }
    }
};

// ============================================================
//  ORDENAMIENTOS
// ============================================================
namespace Ord {

void intercambioDirecto(vector<Producto>& v) {
    for (size_t i = 0; i + 1 < v.size(); i++)
        for (size_t j = 0; j + 1 < v.size() - i; j++)
            if (v[j].precio > v[j+1].precio) swap(v[j], v[j+1]);
}

void insercion(vector<Producto>& v) {
    for (size_t i = 1; i < v.size(); i++) {
        Producto k = v[i];
        int j = (int)i - 1;
        while (j >= 0 && v[j].precio > k.precio) { v[j+1] = v[j]; j--; }
        v[j+1] = k;
    }
}

void seleccion(vector<Producto>& v) {
    for (size_t i = 0; i + 1 < v.size(); i++) {
        size_t idx = i;
        for (size_t j = i + 1; j < v.size(); j++)
            if (v[j].precio < v[idx].precio) idx = j;
        if (idx != i) swap(v[i], v[idx]);
    }
}

int particion(vector<Producto>& v, int lo, int hi) {
    double piv = v[hi].precio;
    int i = lo - 1;
    for (int j = lo; j < hi; j++)
        if (v[j].precio <= piv) { i++; swap(v[i], v[j]); }
    swap(v[i+1], v[hi]);
    return i + 1;
}
void quickRec(vector<Producto>& v, int lo, int hi) {
    if (lo < hi) {
        int p = particion(v, lo, hi);
        quickRec(v, lo, p-1);
        quickRec(v, p+1, hi);
    }
}
void quicksort(vector<Producto>& v) { if (!v.empty()) quickRec(v, 0, (int)v.size()-1); }

void merge(vector<Producto>& v, int l, int m, int r) {
    vector<Producto> L(v.begin()+l, v.begin()+m+1);
    vector<Producto> R(v.begin()+m+1, v.begin()+r+1);
    size_t i = 0, j = 0;
    int k = l;
    while (i < L.size() && j < R.size())
        v[k++] = (compararCadena(L[i].nombre, R[j].nombre) <= 0) ? L[i++] : R[j++];
    while (i < L.size()) v[k++] = L[i++];
    while (j < R.size()) v[k++] = R[j++];
}
void mergeRec(vector<Producto>& v, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeRec(v, l, m);
        mergeRec(v, m+1, r);
        merge(v, l, m, r);
    }
}
void mergesort(vector<Producto>& v) { if (!v.empty()) mergeRec(v, 0, (int)v.size()-1); }

} // namespace Ord

// ============================================================
// 4. BÚSQUEDAS
// ============================================================
namespace Busq {

int secuencial(const vector<Producto>& v, int id) {
    for (size_t i = 0; i < v.size(); i++)
        if (v[i].id == id) return (int)i;
    return -1;
}

int binaria(const vector<Producto>& v, int id) {
    int lo = 0, hi = (int)v.size() - 1;
    while (lo <= hi) {
        int m = lo + (hi - lo) / 2;
        if (v[m].id == id) return m;
        if (v[m].id < id) lo = m + 1;
        else hi = m - 1;
    }
    return -1;
}

int transferencia(const vector<Producto>& v, int id) {
    int n = (int)v.size();
    if (n == 0) return -1;
    int bloque = (int)sqrt((double)n);
    if (bloque < 1) bloque = 1;
    int i = 0;
    while (i + bloque < n && v[i + bloque].id <= id) i += bloque;
    int fin = min(i + bloque, n - 1);
    for (int k = i; k <= fin; k++)
        if (v[k].id == id) return k;
    return -1;
}

} // namespace Busq

// ============================================================
// CATÁLOGO
// ============================================================
class Catalogo {
public:
    set<string> categorias;
    map<string, int> stockPorCategoria;
    map<string, int> productosPorCategoria;

    void registrar(const Producto& p) {
        string cat(p.categoria);
        categorias.insert(cat);
        stockPorCategoria[cat] += p.stock;
        productosPorCategoria[cat]++;
    }

    void mostrar() const {
        cout << "\n--- Catalogo por categoria ---\n";
        set<string>::const_iterator it;
        for (it = categorias.begin(); it != categorias.end(); ++it) {
            string c = *it;
            int s = 0, q = 0;
            map<string,int>::const_iterator it1 = stockPorCategoria.find(c);
            if (it1 != stockPorCategoria.end()) s = it1->second;
            map<string,int>::const_iterator it2 = productosPorCategoria.find(c);
            if (it2 != productosPorCategoria.end()) q = it2->second;
            cout << "  * " << c << " | stock:" << s << " | productos:" << q << "\n";
        }
    }
};

// ============================================================
// COMPARADORES
// ============================================================
bool cmpPorId(const Producto& a, const Producto& b) { return a.id < b.id; }
bool cmpPrioridad(const PedidoProveedor& a, const PedidoProveedor& b) {
    return a.prioridad < b.prioridad;
}

// ============================================================
// SISTEMA PRINCIPAL
// ============================================================
class SistemaPOPS {
    ArbolProductos arbol;
    GrafoProveedores grafo;
    ListaDoble historialVentas;
    ListaSimple historialDevoluciones;
    PilaDevoluciones pilaDevolucionesPendientes;
    ColaClientes colaClientes;
    ColaPrioridadPedidos pedidos;
    Catalogo catalogo;

    map<int, Proveedor> proveedores;
    map<int, Cliente> clientes;
    map<int, Empleado> empleados;
    MapaPropio<int, int> comprasPorCliente;

    vector<Venta> ventasVector;
    vector<Producto> productosVector;

    double caja;
    int contadorVentas;
    int contadorDevoluciones;
    int contadorClientes; // Para el ID automatico

public:
    SistemaPOPS() : caja(0.0), contadorVentas(1), contadorDevoluciones(1), contadorClientes(1) {}

    Fecha hoy() const {
        time_t t = time(NULL);
        tm* lt = localtime(&t);
        Fecha f;
        f.dia = lt->tm_mday;
        f.mes = lt->tm_mon + 1;
        f.anio = lt->tm_year + 1900;
        return f;
    }

    bool caducado(const Fecha& f) const {
        Fecha h = hoy();
        if (f.anio != h.anio) return f.anio < h.anio;
        if (f.mes != h.mes)   return f.mes < h.mes;
        return f.dia < h.dia;
    }

    void actualizarEstado(Producto& p) {
        if (caducado(p.caducidad)) p.estado.codigoEstado = 2;
        else if (p.stock == 0)     p.estado.codigoEstado = 1;
        else                       p.estado.codigoEstado = 0;
    }

    // ---------------- AGREGAR PRODUCTO INTERNO ----------------
    void agregarProducto(int id, const char* nom, const char* cat, double prec,
                         int stk, int stkMin, Fecha cad, int idProv, double costo) {
        Producto p;
        p.id = id;
        copiarCadena(p.nombre, nom, sizeof(p.nombre));
        copiarCadena(p.categoria, cat, sizeof(p.categoria));
        p.precio = prec;
        p.stock = stk;
        p.stockMinimo = stkMin;
        p.caducidad = cad;
        p.idProveedor = idProv;
        actualizarEstado(p);
        arbol.insertar(p);
        productosVector.push_back(p);
        grafo.agregar(idProv, id, costo);
        catalogo.registrar(p);
    }

    // ---------------- ALTA MANUAL DE PRODUCTO ----------------
    void altaProductoManual() {
        Producto p;
        cout << "\n--- ALTA DE NUEVO PRODUCTO ---\n";
        
        cin.clear(); 
        cin.ignore(10000, '\n'); 

        cout << "ID del producto (ej. 109): "; cin >> p.id;
        if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); cout << "Error: ID invalido.\n"; return; }
        
        if (arbol.buscar(p.id)) {
            cout << "Error: Ya existe un producto con ese ID.\n";
            return;
        }
        
        cout << "Nombre del producto (ej. Marcadores de color): "; cin >> ws; cin.getline(p.nombre, sizeof(p.nombre));
        cout << "Categoria (ej. Escritura): "; cin.getline(p.categoria, sizeof(p.categoria));
        
        cout << "Precio (ej. 15.50): "; cin >> p.precio;
        if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); cout << "Error: Precio invalido.\n"; return; }
        
        cout << "Cantidad en stock inicial (ej. 20): "; cin >> p.stock;
        if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); cout << "Error: Stock invalido.\n"; return; }
        
        cout << "Cantidad minima para hacer pedido (ej. 5): "; cin >> p.stockMinimo;
        if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); cout << "Error: Stock minimo invalido.\n"; return; }
        
        cin.clear();
        cin.ignore(10000, '\n');

        cout << "\n>>> INSTRUCCIONES PARA LA FECHA <<<\n";
        cout << "Ingresa el Dia, luego el Mes y luego el Anio. Siempre separados por un Enter.\n";
        cout << "Si el producto caduca el 31 de Diciembre de 2027, debes escribir:\n";
        cout << "Dia: 31\nMes: 12\nAnio: 2027\n\n";
        
        cout << "Dia: "; cin >> p.caducidad.dia;
        cout << "Mes: "; cin >> p.caducidad.mes;
        cout << "Anio: "; cin >> p.caducidad.anio;
        
        if (cin.fail()) { 
            cin.clear(); cin.ignore(10000, '\n'); 
            cout << "Error: Fecha invalida. Debes ingresar numeros.\n"; 
            return; 
        }
        
        cout << "ID del Proveedor (1, 2 o 3): "; 
        cin >> p.idProveedor;
        
        while (cin.fail() || proveedores.find(p.idProveedor) == proveedores.end()) {
            if (cin.fail()) {
                cin.clear(); 
                cin.ignore(10000, '\n'); 
                cout << "Error: Debes ingresar un numero. Intenta de nuevo: ";
            } else {
                cout << "Proveedor no existe. Ingrese otro ID (1, 2 o 3): ";
            }
            cin >> p.idProveedor;
        }
        
        double costo;
        cout << "Costo de suministro del proveedor (ej. 5.50): "; cin >> costo;
        if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); cout << "Error: Costo invalido.\n"; return; }

        agregarProducto(p.id, p.nombre, p.categoria, p.precio, p.stock, 
                        p.stockMinimo, p.caducidad, p.idProveedor, costo);

        cout << "Producto agregado y registrado correctamente en el sistema.\n";
    }

    void sincronizarEspejo(int id) {
        Producto* p = arbol.buscar(id);
        if (!p) return;
        actualizarEstado(*p);
        for (size_t i = 0; i < productosVector.size(); i++) {
            if (productosVector[i].id == id) { productosVector[i] = *p; return; }
        }
    }

    void cargarDatosIniciales() {
        Proveedor p1; p1.id = 1;
        copiarCadena(p1.empresa, "Papelera Nacional S.A.", sizeof(p1.empresa));
        copiarCadena(p1.telefono, "555-1111", sizeof(p1.telefono));
        copiarCadena(p1.correo, "ventas@pnacional.com", sizeof(p1.correo));
        proveedores[1] = p1;

        Proveedor p2; p2.id = 2;
        copiarCadena(p2.empresa, "Lapices y Mas", sizeof(p2.empresa));
        copiarCadena(p2.telefono, "555-2222", sizeof(p2.telefono));
        copiarCadena(p2.correo, "contacto@lapices.com", sizeof(p2.correo));
        proveedores[2] = p2;

        Proveedor p3; p3.id = 3;
        copiarCadena(p3.empresa, "Distribuidora Escolar", sizeof(p3.empresa));
        copiarCadena(p3.telefono, "555-3333", sizeof(p3.telefono));
        copiarCadena(p3.correo, "info@disescolar.com", sizeof(p3.correo));
        proveedores[3] = p3;

        Fecha f1 = {31,12,2027}; agregarProducto(101, "Cuaderno profesional", "Cuadernos", 35.50, 20, 5, f1, 1, 8.5);
        Fecha f2 = {31,12,2030}; agregarProducto(102, "Lapiz HB", "Escritura", 5.00, 100, 20, f2, 2, 1.2);
        Fecha f3 = {31,12,2029}; agregarProducto(103, "Goma de borrar", "Escritura", 7.50, 50, 10, f3, 2, 1.8);
        Fecha f4 = {15,6,2026};  agregarProducto(104, "Pegamento blanco", "Adhesivos", 18.00, 15, 5, f4, 1, 4.5);
        Fecha f5 = {10,5,2026};  agregarProducto(105, "Tinta para plumon", "Escritura", 22.00, 8, 3, f5, 3, 6.0);
        Fecha f6 = {31,12,2030}; agregarProducto(106, "Regla 30cm", "Geometria", 12.00, 40, 10, f6, 3, 3.0);
        Fecha f7 = {20,10,2027}; agregarProducto(107, "Marcador permanente", "Escritura", 15.00, 25, 5, f7, 2, 4.0);
        Fecha f8 = {31,12,2028}; agregarProducto(108, "Colores x12", "Colores", 45.00, 12, 4, f8, 1, 12.0);

        // Clientes iniciales (sin ID manual, se asigna auto en contadorClientes)
        Cliente c1; c1.id = contadorClientes++;
        copiarCadena(c1.nombre, "Ana Lopez", sizeof(c1.nombre));
        copiarCadena(c1.telefono, "5558000", sizeof(c1.telefono));
        clientes[c1.id] = c1;

        Cliente c2; c2.id = contadorClientes++;
        copiarCadena(c2.nombre, "Carlos Ruiz", sizeof(c2.nombre));
        copiarCadena(c2.telefono, "5558001", sizeof(c2.telefono));
        clientes[c2.id] = c2;

        Cliente c3; c3.id = contadorClientes++;
        copiarCadena(c3.nombre, "Maria Perez", sizeof(c3.nombre));
        copiarCadena(c3.telefono, "5558002", sizeof(c3.telefono));
        clientes[c3.id] = c3;

        Empleado e1; e1.id = 1;
        copiarCadena(e1.nombre, "Juan Vendedor", sizeof(e1.nombre));
        copiarCadena(e1.puesto, "Cajero", sizeof(e1.puesto));
        e1.salario = 8000;
        empleados[1] = e1;

        Empleado e2; e2.id = 2;
        copiarCadena(e2.nombre, "Lucia Gerente", sizeof(e2.nombre));
        copiarCadena(e2.puesto, "Gerente", sizeof(e2.puesto));
        e2.salario = 15000;
        empleados[2] = e2;

        Empleado e3; e3.id = 3;
        copiarCadena(e3.nombre, "Pedro Auxiliar", sizeof(e3.nombre));
        copiarCadena(e3.puesto, "Auxiliar", sizeof(e3.puesto));
        e3.salario = 6000;
        empleados[3] = e3;
    }

    void mostrarProductos() {
        vector<Producto> lista = arbol.listar();
        cout << "\n===== LISTA DE PRODUCTOS =====\n";
        cout << left << setw(5) << "ID" << setw(24) << "Nombre"
             << setw(14) << "Categoria" << setw(10) << "Precio"
             << setw(7) << "Stock" << setw(6) << "Min"
             << setw(12) << "Caducidad" << "Estado\n";
        cout << string(90, '-') << "\n";
        for (size_t i = 0; i < lista.size(); i++) {
            Producto& p = lista[i];
            cout << left << setw(5) << p.id
                 << setw(24) << p.nombre
                 << setw(14) << p.categoria
                 << setw(10) << fixed << setprecision(2) << p.precio
                 << setw(7) << p.stock
                 << setw(6) << p.stockMinimo
                 << setw(12);
            cout << p.caducidad.dia << "/" << p.caducidad.mes << "/" << p.caducidad.anio;
            if (p.estado.codigoEstado == 2) cout << "  CADUCADO";
            else if (p.estado.codigoEstado == 1) cout << "  AGOTADO";
            else cout << "  OK";
            cout << "\n";
        }
    }

    void mostrarCatalogo() const { catalogo.mostrar(); }
    void mostrarGrafo() const { grafo.mostrar(); }
    void mostrarVentas() const { historialVentas.mostrar(); }
    void mostrarDevoluciones() const { historialDevoluciones.mostrar(); }
    void mostrarCaja() const { cout << "\nCaja actual: $" << fixed << setprecision(2) << caja << "\n"; }
    void mostrarComprasPorCliente() const {
        cout << "\n--- Compras por cliente (mapa propio) ---\n";
        comprasPorCliente.mostrar();
    }

    void menuOrdenamientos() {
        if (productosVector.empty()) { cout << "Sin productos.\n"; return; }
        cout << "\n--- Ordenamientos ---\n"
             << "1. Intercambio directo\n"
             << "2. Insercion\n"
             << "3. Seleccion directa\n"
             << "4. Quicksort\n"
             << "5. Merge Sort\n"
             << "0. Cancelar\n"
             << "Opcion: ";
        int op; cin >> op;
        if (op == 0) return;
        vector<Producto> copia = productosVector;
        switch (op) {
            case 1: Ord::intercambioDirecto(copia); break;
            case 2: Ord::insercion(copia); break;
            case 3: Ord::seleccion(copia); break;
            case 4: Ord::quicksort(copia); break;
            case 5: Ord::mergesort(copia); break;
            default: cout << "Opcion invalida.\n"; return;
        }
        cout << "\n--- Resultado ---\n";
        for (size_t i = 0; i < copia.size(); i++)
            cout << "  #" << copia[i].id << " " << copia[i].nombre << " | $"
                 << fixed << setprecision(2) << copia[i].precio
                 << " | stock:" << copia[i].stock << "\n";
    }

    void menuBusquedas() {
        cout << "\n--- Busquedas ---\n"
             << "1. Secuencial\n2. Binaria\n3. Por transferencia\n0. Cancelar\nOpcion: ";
        int op; cin >> op;
        if (op == 0) return;
        cout << "ID del producto: ";
        int id; cin >> id;

        vector<Producto> ordenado = productosVector;
        sort(ordenado.begin(), ordenado.end(), cmpPorId);

        int idx = -1;
        const vector<Producto>* fuente = &productosVector;
        switch (op) {
            case 1: idx = Busq::secuencial(productosVector, id); break;
            case 2: idx = Busq::binaria(ordenado, id); fuente = &ordenado; break;
            case 3: idx = Busq::transferencia(ordenado, id); fuente = &ordenado; break;
            default: cout << "Opcion invalida.\n"; return;
        }
        if (idx >= 0) {
            const Producto& p = (*fuente)[idx];
            cout << "Encontrado: #" << p.id << " " << p.nombre
                 << " | $" << p.precio << " | stock:" << p.stock << "\n";
        } else {
            cout << "Producto no encontrado.\n";
        }
    }

    // ---------------- REALIZAR VENTA  ----------------
    void realizarVenta() {
        cout << "\n--- Nueva Venta ---\n";
        
        cout << "Clientes disponibles:\n";
        map<int, Cliente>::iterator itC;
        for (itC = clientes.begin(); itC != clientes.end(); ++itC) {
            cout << "  ID: " << itC->first << " - " << itC->second.nombre << "\n";
        }
        
        cout << "ID Cliente: ";
        int idCliente; cin >> idCliente;
        
        cout << "ID Empleado: ";
        int idEmpleado; cin >> idEmpleado;
        if (clientes.find(idCliente) == clientes.end()) { cout << "Cliente no existe.\n"; return; }
        if (empleados.find(idEmpleado) == empleados.end()) { cout << "Empleado no existe.\n"; return; }

        Venta v;
        v.id = contadorVentas;
        v.idCliente = idCliente;
        v.idEmpleado = idEmpleado;
        v.fecha = hoy();
        v.total = 0.0;

        while (true) {
            cout << "ID Producto (0 para terminar): ";
            int idP; cin >> idP;
            if (idP == 0) break;

            Producto* p = arbol.buscar(idP);
            
            if (!p) {
                cout << "El producto #" << idP << " NO existe en la tienda.\n";
                cout << "Deseas agregarlo ahora para poder venderlo? (s/n): ";
                char resp; cin >> resp;
                if (resp == 's' || resp == 'S') {
                    altaProductoManual(); 
                    p = arbol.buscar(idP);
                    if (!p) {
                        cout << "No se pudo agregar el producto (quiza no usaste el mismo ID).\n";
                        continue;
                    }
                } else {
                    cout << "No se puede vender un producto inexistente.\n";
                    continue;
                }
            }

            cout << "Cantidad: ";
            int cant; cin >> cant;

            if (caducado(p->caducidad)) {
                cout << "** Producto CADUCADO, no se puede vender. **\n";
                continue;
            }
            if (cant <= 0) { cout << "Cantidad invalida.\n"; continue; }
            if (p->stock < cant) {
                cout << "** Stock insuficiente. Disponible: " << p->stock << " **\n";
                continue;
            }

            p->stock -= cant;
            actualizarEstado(*p);
            v.total += p->precio * cant;
            ItemVenta it; it.idProducto = idP; it.cantidad = cant;
            v.items.push_back(it);
            sincronizarEspejo(idP);
            cout << "Agregado: " << p->nombre << " x" << cant << "\n";

            if (p->stock <= p->stockMinimo) {
                cout << "*** Stock minimo alcanzado en " << p->nombre << " ***\n";
                PedidoProveedor pp;
                pp.idProducto = p->id;
                copiarCadena(pp.nombreProducto, p->nombre, sizeof(pp.nombreProducto));
                copiarCadena(pp.empresaProveedor, proveedores[p->idProveedor].empresa, sizeof(pp.empresaProveedor));
                pp.cantidad = max(p->stockMinimo * 2, 10);
                pp.prioridad = p->stock;
                pedidos.agregar(pp);
            }
        }

        if (v.items.empty()) { cout << "Venta cancelada.\n"; return; }

        caja += v.total;
        ventasVector.push_back(v);
        historialVentas.agregar(v);

        int prev = 0;
        if (!comprasPorCliente.obtener(idCliente, prev)) prev = 0;
        comprasPorCliente.insertar(idCliente, prev + 1);

        contadorVentas++;
        cout << "\nVenta registrada. Total: $" << fixed << setprecision(2)
             << v.total << " | Caja: $" << caja << "\n";
    }

    void realizarDevolucion() {
        cout << "\n--- Devolucion ---\n";
        cout << "ID Venta: ";
        int idVenta; cin >> idVenta;
        cout << "ID Producto: ";
        int idProd; cin >> idProd;
        cout << "Cantidad: ";
        int cant; cin >> cant;

        for (size_t i = 0; i < ventasVector.size(); i++) {
            Venta& v = ventasVector[i];
            if (v.id != idVenta) continue;
            for (size_t j = 0; j < v.items.size(); j++) {
                if (v.items[j].idProducto != idProd) continue;
                if (cant <= 0 || cant > v.items[j].cantidad) {
                    cout << "Cantidad invalida (vendidos: " << v.items[j].cantidad << ").\n";
                    return;
                }
                Producto* p = arbol.buscar(idProd);
                if (!p) return;
                p->stock += cant;
                actualizarEstado(*p);
                sincronizarEspejo(idProd);

                Devolucion d;
                d.idVenta = idVenta;
                d.idProducto = idProd;
                d.cantidad = cant;
                copiarCadena(d.motivo, "Devolucion cliente", sizeof(d.motivo));
                d.fecha = hoy();

                pilaDevolucionesPendientes.push(d);
                caja -= cant * p->precio;
                cout << "Devolucion encolada en pila. Caja: $" << fixed
                     << setprecision(2) << caja << "\n";
                return;
            }
        }
        cout << "Venta/producto no encontrado.\n";
    }

    void procesarDevolucionPendiente() {
        Devolucion d;
        if (!pilaDevolucionesPendientes.pop(d)) {
            cout << "No hay devoluciones pendientes.\n";
            return;
        }
        historialDevoluciones.agregar(d);
        cout << "Devolucion #" << contadorDevoluciones
             << " procesada (venta #" << d.idVenta << ").\n";
        contadorDevoluciones++;
    }

    void verPedidosPendientes() const {
        cout << "\n--- Pedidos pendientes ---\n";
        vector<PedidoProveedor> lista = pedidos.aVector();
        if (lista.empty()) { cout << "  (ninguno)\n"; return; }
        sort(lista.begin(), lista.end(), cmpPrioridad);
        for (size_t i = 0; i < lista.size(); i++)
            cout << "  " << lista[i].nombreProducto
                 << " | stock:" << lista[i].prioridad
                 << " | pedir:" << lista[i].cantidad
                 << " | prov:" << lista[i].empresaProveedor << "\n";
    }

    // ---------------- EXPORTAR PEDIDOS (MEJORADO CON LECTURA EN CONSOLA) ----------------
    void exportarPedidos() {
        ofstream f("pedido_proveedores.txt");
        if (!f) { cout << "No se pudo crear archivo.\n"; return; }
        Fecha h = hoy();
        
        // Formato mejorado
        f << "==============================================================\n";
        f << "EMPRESA: PAPELERIA POPS\n";
        f << "FECHA DE PEDIDO: " << h.dia << "/" << h.mes << "/" << h.anio << "\n";
        f << "==============================================================\n";
        f << left << setw(30) << "PRODUCTO"
             << setw(10) << "CANTIDAD"
             << setw(25) << "PROVEEDOR" << "\n";
        f << "--------------------------------------------------------------\n";

        vector<PedidoProveedor> lista = pedidos.aVector();
        sort(lista.begin(), lista.end(), cmpPrioridad);
        for (size_t i = 0; i < lista.size(); i++)
            f << left << setw(30) << lista[i].nombreProducto
                 << setw(10) << lista[i].cantidad
                 << setw(25) << lista[i].empresaProveedor << "\n";
        f << "==============================================================\n";
        f << "TOTAL DE LINEAS: " << lista.size() << "\n";
        f.close();
        
        cout << "\n¡Archivo 'pedido_proveedores.txt' generado exitosamente!\n";
        cout << ">> Aqui tienes el contenido del archivo para que lo verifiques:\n\n";
        
        // Leer y mostrar el archivo en consola
        ifstream leer("pedido_proveedores.txt");
        if (leer) {
            string linea;
            while (getline(leer, linea)) {
                cout << linea << "\n";
            }
            leer.close();
        }
    }

    void cargarProductosDesdeArchivo() {
        cout << "Nombre del archivo .txt: ";
        string nombre; cin >> nombre;
        ifstream f(nombre.c_str());
        if (!f) { cout << "No se pudo abrir.\n"; return; }
        string linea;
        int cargados = 0;
        while (getline(f, linea)) {
            if (linea.empty() || linea[0] == '#') continue;
            stringstream ss(linea);
            vector<string> campos;
            string tok;
            while (getline(ss, tok, ',')) campos.push_back(tok);
            if (campos.size() < 11) { cout << "  linea invalida: " << linea << "\n"; continue; }
            try {
                int id = atoi(campos[0].c_str());
                string nom = campos[1];
                string cat = campos[2];
                double prec = atof(campos[3].c_str());
                int stk = atoi(campos[4].c_str());
                int stkMin = atoi(campos[5].c_str());
                int dia = atoi(campos[6].c_str());
                int mes = atoi(campos[7].c_str());
                int anio = atoi(campos[8].c_str());
                int idProv = atoi(campos[9].c_str());
                double costo = atof(campos[10].c_str());
                if (arbol.buscar(id)) { cout << "  ID " << id << " ya existe.\n"; continue; }
                Fecha f2; f2.dia = dia; f2.mes = mes; f2.anio = anio;
                agregarProducto(id, nom.c_str(), cat.c_str(), prec,
                                stk, stkMin, f2, idProv, costo);
                cargados++;
            } catch (...) {
                cout << "  error de formato: " << linea << "\n";
            }
        }
        cout << "Productos cargados: " << cargados << "\n";
    }

    // ---------------- ALTA CLIENTE ----------------
    void altaCliente() {
        Cliente c;
        c.id = contadorClientes++; 
        cout << "\n--- NUEVO CLIENTE (ID asignado: " << c.id << ") ---\n";
        cout << "Nombre: "; cin >> ws; cin.getline(c.nombre, sizeof(c.nombre));
        
        cout << "Telefono (exactamente 7 digitos, ej. 1234567): "; 
        cin >> c.telefono;
        while (strlen(c.telefono) != 7) {
            cout << "Error: El telefono debe tener exactamente 7 digitos.\n";
            cout << "Intenta de nuevo (ej. 1234567): ";
            cin >> c.telefono;
        }
        
        clientes[c.id] = c;
        cout << "Cliente registrado.\n";
    }

    // ---------------- ALTA PROVEEDOR (AUTOMATICO) ----------------
    void altaProveedor() {
        cout << "\n--- NUEVO PROVEEDOR ---\n";
        cout << "El sistema cuenta con los siguientes proveedores predefinidos:\n";
        
        Proveedor opciones[3];
        opciones[0].id = 1;
        copiarCadena(opciones[0].empresa, "Papelera Nacional S.A.", sizeof(opciones[0].empresa));
        copiarCadena(opciones[0].telefono, "555-1111", sizeof(opciones[0].telefono));
        copiarCadena(opciones[0].correo, "ventas@pnacional.com", sizeof(opciones[0].correo));
        
        opciones[1].id = 2;
        copiarCadena(opciones[1].empresa, "Lapices y Mas", sizeof(opciones[1].empresa));
        copiarCadena(opciones[1].telefono, "555-2222", sizeof(opciones[1].telefono));
        copiarCadena(opciones[1].correo, "contacto@lapices.com", sizeof(opciones[1].correo));
        
        opciones[2].id = 3;
        copiarCadena(opciones[2].empresa, "Distribuidora Escolar", sizeof(opciones[2].empresa));
        copiarCadena(opciones[2].telefono, "555-3333", sizeof(opciones[2].telefono));
        copiarCadena(opciones[2].correo, "info@disescolar.com", sizeof(opciones[2].correo));

        for (int i = 0; i < 3; i++) {
            cout << "\nOpcion " << i+1 << ":\n";
            cout << "  ID: " << opciones[i].id << "\n";
            cout << "  Empresa: " << opciones[i].empresa << "\n";
            cout << "  Telefono: " << opciones[i].telefono << "\n";
            cout << "  Correo: " << opciones[i].correo << "\n";
            cout << "Deseas confirmar y registrar este proveedor? (s/n): ";
            char resp; cin >> resp;
            if (resp == 's' || resp == 'S') {
                proveedores[opciones[i].id] = opciones[i];
                cout << "Proveedor '" << opciones[i].empresa << "' registrado.\n";
            } else {
                cout << "Proveedor '" << opciones[i].empresa << "' omitido.\n";
            }
        }
    }

    // ---------------- ALTA EMPLEADO (PUESTO AUTOMATICO Y TEL 7 DIGITOS) ----------------
    void altaEmpleado() {
        Empleado e;
        cout << "ID: "; cin >> e.id;
        cout << "Nombre: "; cin >> ws; cin.getline(e.nombre, sizeof(e.nombre));
        
        int opPuesto;
        cout << "\nSelecciona el Puesto:\n";
        cout << "1. Cajero (Atencion al cliente y ventas)\n";
        cout << "2. Gerente (Administracion y supervision)\n";
        cout << "Opcion: ";
        cin >> opPuesto;
        
        if (opPuesto == 1) {
            copiarCadena(e.puesto, "Cajero", sizeof(e.puesto));
        } else if (opPuesto == 2) {
            copiarCadena(e.puesto, "Gerente", sizeof(e.puesto));
        } else {
            cout << "Opcion invalida. Se asignara 'Auxiliar' por defecto.\n";
            copiarCadena(e.puesto, "Auxiliar", sizeof(e.puesto));
        }

        cout << "Salario: "; cin >> e.salario;
        
        empleados[e.id] = e;
        cout << "Empleado registrado.\n";
    }

    void encolarCliente() {
        cout << "ID cliente: ";
        int id; cin >> id;
        if (clientes.find(id) == clientes.end()) { cout << "Cliente no existe.\n"; return; }
        colaClientes.encolar(clientes[id]);
        cout << "Cliente en fila. En espera: " << colaClientes.tam() << "\n";
    }

    void atenderCliente() {
        Cliente c;
        if (!colaClientes.desencolar(c)) { cout << "Fila vacia.\n"; return; }
        cout << "Atendiendo a " << c.nombre << " (" << c.telefono << ")\n";
    }

    // ---------------- MENU ----------------
    void menu() {
        int op;
        do {
            cout << "\n==========================================\n";
            cout << "     PAPELERIA POPS - SISTEMA DE GESTION\n";
            cout << "==========================================\n";
            cout << "1.  Agregar Empleado\n";
            cout << "2.  Agregar Proveedor\n";
            cout << "3.  Agregar Cliente\n";
            cout << "4.  Ver productos\n";
            cout << "5.  Ver catalogo por categoria\n";
            cout << "6.  Ver grafo proveedor->producto\n";
            cout << "7.  Ordenamientos\n";
            cout << "8.  Busquedas\n";
            cout << "9.  Realizar venta\n";
            cout << "10. Realizar devolucion\n";
            cout << "11. Procesar devolucion pendiente\n";
            cout << "12. Ver pedidos pendientes\n";
            cout << "13. Exportar pedido a archivo .txt (y ver en consola)\n";
            cout << "14. Cargar productos desde .txt\n";
            cout << "15. Ver historial de ventas\n";
            cout << "16. Ver devoluciones procesadas\n";
            cout << "17. Ver caja\n";
            cout << "18. Ver compras por cliente\n";
            cout << "19. Encolar cliente\n";
            cout << "20. Atender siguiente cliente\n";
            cout << "21. Agregar producto manualmente\n";
            cout << "0.  Salir\n";
            cout << "Opcion: ";
            cin >> op;

            switch (op) {
                case 1:  altaEmpleado(); break;
                case 2:  altaProveedor(); break;
                case 3:  altaCliente(); break;
                case 4:  mostrarProductos(); break;
                case 5:  mostrarCatalogo(); break;
                case 6:  mostrarGrafo(); break;
                case 7:  menuOrdenamientos(); break;
                case 8:  menuBusquedas(); break;
                case 9:  realizarVenta(); break;
                case 10: realizarDevolucion(); break;
                case 11: procesarDevolucionPendiente(); break;
                case 12: verPedidosPendientes(); break;
                case 13: exportarPedidos(); break;
                case 14: cargarProductosDesdeArchivo(); break;
                case 15: mostrarVentas(); break;
                case 16: mostrarDevoluciones(); break;
                case 17: mostrarCaja(); break;
                case 18: mostrarComprasPorCliente(); break;
                case 19: encolarCliente(); break;
                case 20: atenderCliente(); break;
                case 21: altaProductoManual(); break;
                case 0:  cout << "Saliendo...\n"; break;
                default: cout << "Opcion invalida.\n";
            }
        } while (op != 0);
    }
};

// ============================================================
int main() {
    SistemaPOPS app;
    app.cargarDatosIniciales();
    app.menu();
    return 0;
}
