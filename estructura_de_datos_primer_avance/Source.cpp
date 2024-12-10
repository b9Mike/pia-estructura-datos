#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "resource.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <ctime>
#include <string>
#include <vector>
#include <cctype>
#include <sstream>
#include <commctrl.h>

using namespace std;

int TipoUsuario = -1;
int usuarioLogueado = 0;
string nombreUsuario = "";

//variables para saber el ultimo id registrado
int ultimoID = 0;
int ultimoIDTienda = 0;
int ultimoIdCompra = 0;
int ultimoIdEnvio = 0;

int idClienteEditar = 0;
int idTiendaEditar = 0;

string idProductoComprar = "null";
double precioUnitario = 0.0;
int idTiendaComprar = 0;
string fechaAprox = "";

string clienteComprar;
string tiendaComprar;

int idEnvioActualizar = -1;


// Función para obtener la fecha actual
string obtenerFechaActual();
bool esNumero(const char* cadena);

// Estructura del Cliente (nodo del árbol)
struct Cliente {
    int id;
    string nombre;
    string apellidoPaterno;
    string apellidoMaterno;
    string usuario;
    string contrasenia;
    string fechaRegistro;
    string estatus;
    string fechaCambioEstatus;
    int idUsuarioActualizo; // Nuevo campo para el ID del usuario que actualizó
    int tipoUsuario;
    Cliente* izquierda;
    Cliente* derecha;

    Cliente(int id, const string& nombre, const string& apellidoPaterno, const string& apellidoMaterno, const string& usuario, const string& contrasenia, const string& estatus, const string& fechaActualizo, int idUsuarioActualizo, int tipoUsuario)
        : id(id), nombre(nombre), apellidoPaterno(apellidoPaterno), apellidoMaterno(apellidoMaterno), usuario(usuario), contrasenia(contrasenia), fechaRegistro(obtenerFechaActual()), estatus(estatus),
        fechaCambioEstatus(fechaActualizo), idUsuarioActualizo(idUsuarioActualizo), tipoUsuario(tipoUsuario),
        izquierda(nullptr), derecha(nullptr) {}

    Cliente() : id(0), // Inicializado a 0
        izquierda(nullptr), derecha(nullptr) {}
};

//Estructura de teindas (listas doblemente ligadas)
struct Tienda {
    int id;                       // ID único de la tienda
    string nombre;                // Nombre de la tienda
    string direccion;             // Dirección de la tienda
    string estatus;               // Estatus de la tienda ("Activa", "Inactiva", etc.)
    string fechaCambioEstatus;    // Fecha de cambio de estatus
    Tienda* anterior;             // Puntero al nodo anterior en la lista doblemente ligada
    Tienda* siguiente;            // Puntero al siguiente nodo en la lista doblemente ligada

    // Constructor de Tienda
    Tienda(int id, const string& nombre, const string& direccion)
        : id(id), nombre(nombre), direccion(direccion), estatus("Activa"),
        fechaCambioEstatus(obtenerFechaActual()), anterior(nullptr), siguiente(nullptr) {}
};

// Estructura de Producto (listas doblemente ligadas)
struct Producto {
    string codigo;                   // Código único del producto
    string nombre;                // Nombre del producto
    double precio;                // Precio del producto
    double costo;                 // Costo del producto
    int existencias;              // Cantidad en existencia
    int tienda;                     // id a la tienda que ofrece el producto
    string estatus;               // Estatus del producto ("Disponible", "No disponible", etc.)
    string fechaCambioEstatus;    // Fecha de cambio de estatus
    Producto* anterior;           // Puntero al nodo anterior en la lista doblemente ligada
    Producto* siguiente;          // Puntero al siguiente nodo en la lista doblemente ligada

    // Constructor de Producto
    Producto(const string& codigo, const string& nombre, double precio, double costo, int existencias, int tienda)
        : codigo(codigo), nombre(nombre), precio(precio), costo(costo), existencias(existencias),
        tienda(tienda), estatus("Disponible"), fechaCambioEstatus(obtenerFechaActual()),
        anterior(nullptr), siguiente(nullptr) {
        // Validaciones
        if (precio < 0 || costo < 0 || existencias < 0) {
            MessageBox(nullptr, "El precio, costo y existencias no pueden ser negativos", "Error", MB_OK | MB_ICONERROR);
            throw std::invalid_argument("Los valores no pueden ser negativos");
        }
        if (nombre.empty()) {
            MessageBox(nullptr, "El nombre del producto no puede estar vacío", "Error", MB_OK | MB_ICONERROR);
            throw std::invalid_argument("El nombre no puede estar vacío");
        }
    }
};

struct Compra {
    int id;
    string producto;
    string direccion;
    int cantidad;
    double precioUnitario;
    double precioTotal;
    string fechaCompra;
    string fechaAprox;
    string estatus;
    string cliente;
    string tienda;
    int idTienda;
    int idCliente;
    string idProducto;
    Compra* anterior;
    Compra* siguiente;

    Compra(int id, const string& producto, const string& direccion, int cantidad, double precioUnitario, double precioTotal,
        const string& fechaAprox, const string& cliente, const string& tienda, int idTienda, int idCliente, const string& idProducto)
        : id(id), producto(producto), direccion(direccion), cantidad(cantidad), precioUnitario(precioUnitario), precioTotal(precioTotal),
        fechaCompra(obtenerFechaActual()), fechaAprox(fechaAprox), estatus("Compra realizada"), cliente(cliente), tienda(tienda), idTienda(idTienda), idCliente(idCliente),
        idProducto(idProducto), anterior(nullptr), siguiente(nullptr){}

};

struct Envios {
    int id;                       
    int idCompra;                
    string producto;             
    string direccion;            
    string estatus;              
    string fechaActualizacion;
    string cliente;
    Envios* anterior;            
    Envios* siguiente;

    // Constructor de Tienda
    Envios(int id, int idCompra,const string& producto, const string& direccion, const string& cliente)
        : id(id), idCompra(idCompra), producto(producto), direccion(direccion),estatus("En camino"), fechaActualizacion(obtenerFechaActual()),
        cliente(cliente), anterior(nullptr), siguiente(nullptr) {}
};


//----------------------------------------------------------------CLIENTES-----------------------------------------------------------------
// Función para agregar un cliente al árbol
Cliente* agregarCliente(Cliente* raiz, int id, const string& nombre, const string& apellidoPaterno, const string& apellidoMaterno, const string& usuario, const string& contrasenia, const string& estatus, const string& fechaActualizo, int idUsuarioActualizo, int tipoUsuario);

// Función para busquedaBinaria 
Cliente* busquedaBinariaCliente(Cliente* raiz, int id);

// Función para mostrar los clientes en orden (ID en orden ascendente)
void mostrarClientesEnOrden(Cliente* raiz, HWND hwndListBox);

// Función para cambiar el estatus de un cliente
void editarCliente(Cliente* raiz, int id, const string& nuevoNombre, const string& nuevoApellidoPaterno, const string& nuevoApellidoMaterno, const string& nuevoUsuario, const string& nuevaContrasenia, const string& nuevoEstatus, int tipoUsuario);

// Función para liberar la memoria del árbol
void eliminarArbolClientes(Cliente* raiz);

// Función para guardar los datos en un archivo binario
void guardarDatos(Cliente* raiz, ofstream& archivo);

Cliente* cargarDatos(ifstream& archivo, Cliente* raiz);

Cliente* buscarClientePorID(Cliente* raiz, int clienteID);

Cliente* buscarClientePorUsuario(Cliente* raiz, const string& usuario);

Cliente* raiz = nullptr;


//----------------------------------------------------------------TIENDAS-----------------------------------------------------------------
// Función para guardar tiendas en un archivo binario
void guardarTiendas(Tienda* cabeza, const string& nombreArchivo);

// Función para cargar tiendas desde un archivo binario
Tienda* cargarTiendas(const string& nombreArchivo);

void agregarTienda(Tienda*& cabeza, int id, const string& nombre, const string& direccion);

void eliminarTiendas(Tienda*& cabeza);

void mostrarTiendasEnListBox(Tienda* cabeza, HWND hwndListBox);

void actualizarTienda(Tienda*& cabeza, int idTiendaEditar, HWND hwnd);

void cargarTiendasEnComboBox(HWND hwndComboBox, Tienda* cabezaTiendas);

Tienda* cabezaTienda = cargarTiendas("tiendas.bin");

Tienda* buscarTienda(Tienda* inicio, int id);

//----------------------------------------------------------------PRODUCTOS-----------------------------------------------------------------
void agregarProducto(Producto*& cabeza, const string& codigo, const string& nombre, double precio, double costo, int existencias, int tienda);
void liberarProductos(Producto*& cabeza);
void guardarProductos(Producto* cabeza, const string& nombreArchivo);
Producto* cargarProductos(const string& nombreArchivo);
void mostrarProductosEnListBox(Producto* cabeza, HWND hwndListBox);
void mostrarProductosEnListBoxUsuario(Producto* cabeza, HWND hwndListBox);

Producto* cabezaProductos = cargarProductos("productos.bin");

//----------------------------------------------------------------COMPRA-----------------------------------------------------------------
void agregarCompra(Compra*& cabeza, int id, const string& producto, const string& direccion,
    int cantidad, double precioUnitario, const string& fechaAprox, const string& cliente, const string& tienda,
    int idTienda, int idCliente, const string& idProducto);
void liberarMemoriaCompras(Compra*& cabeza);

void guardarCompras(Compra* cabeza, const string& nombreArchivo);
Compra* cargarCompras(const string& nombreArchivo);

Compra* cabezaCompras = cargarCompras("compras.bin");

void cargarComprasEnListView(Compra* cabezaCompras, HWND hwndListView);
void cargarComprasEnListViewCliente(Compra* cabezaCompras, HWND hwndListView);
void cargarComprasEnListViewTienda(Compra* cabezaCompras, HWND hwndListView, int tienda);
void cargarComprasEnListViewProducto(Compra* cabezaCompras, HWND hwndListView, string producto);


//----------------------------------------------------------------COMPRA-----------------------------------------------------------------
void agregarEnvio(Envios*& cabeza, int id, int idCompra, const string& producto, const string& direccion, const string& cliente);
void guardarEnvios(Envios* cabeza, const string& nombreArchivo);
Envios* cargarEnvios(const string& nombreArchivo);
void liberarMemoriaEnvios(Envios*& cabeza);

void cargarEnviosEnListView(Envios* cabezaEnvios, HWND hwndListView);
void cargarEnviosEnListViewClientes(Envios* cabezaEnvios, HWND hwndListView);

Envios* cabezaEnvios = cargarEnvios("envios.bin");

//-------------------------------------------------------------------METODSO DE ORDENAMIENTO-------------------------------------------------
// funciones para heapsort para ordenar tiendas
void heapify(Tienda** arr, int n, int i);
void heapSort(Tienda** arr, int n);
void ordenarTiendasPorID(Tienda*& cabeza);

// funciones para quicksort para ordenar productos
Producto* partition(Producto* low, Producto* high);
void quickSort(Producto* low, Producto* high);
Producto* findLastNode(Producto* head);
void sortProductos(Producto* cabezaProductos);


INT_PTR CALLBACK fVentanaLogin(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaRegister(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaDashboard(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaRClientes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaEClientes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaRTienda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaETienda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaRProductos(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaEditarProductos(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaReporteCompra(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaReporteCompraPorTienda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaReporteCompraPorProducto(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaReporteEnvios(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


INT_PTR CALLBACK fVentanaDashboardUser(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK fVentanaComprar(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//funcion MAIN
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, PSTR cmdline, int nCmdShow) {

    
    // Cargar datos al iniciar el programa
    ifstream archivoEntrada("clientes.bin", ios::binary);
    if (archivoEntrada) {
        raiz = cargarDatos(archivoEntrada, raiz);
        archivoEntrada.close();
    }
    else {
        raiz = agregarCliente(raiz, 1, "ADMIN", "admin", "null", "admin", "Adminpass", "Activo", obtenerFechaActual(), 1, 1);
    }

    // Crear la ventana de login
    HWND hVentanaLogin = CreateDialog(hInstance, MAKEINTRESOURCE(DLG_LOGIN), NULL, fVentanaLogin);

    ShowWindow(hVentanaLogin, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Guardar datos al cerrar el programa
    ofstream archivoSalida("clientes.bin", ios::binary);
    if (archivoSalida) {
        guardarDatos(raiz, archivoSalida);
        archivoSalida.close();
    }

    //llamar funciones para guardar informacion
    guardarTiendas(cabezaTienda, "tiendas.bin");
    guardarProductos(cabezaProductos, "productos.bin");
    guardarCompras(cabezaCompras, "compras.bin");
    guardarEnvios(cabezaEnvios, "envios.bin");

    //liberar memoria de las listas ligadas y el arbol binario de usuarios
    liberarMemoriaEnvios(cabezaEnvios);
    liberarMemoriaCompras(cabezaCompras);
    liberarProductos(cabezaProductos);
    eliminarTiendas(cabezaTienda);
    eliminarArbolClientes(raiz);


    
    return 0;
}
//================================================================== CALLBACKS =====================================================================

INT_PTR CALLBACK fVentanaLogin(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTN_LOGIN_LOGIN: // Manejar el clic en el botón de inicio de sesión
        {
            // Obtener el texto de los cuadros de texto
            char usuario[20];
            char contrasenia[20];
            bool usuarioAdmin = false;
            GetDlgItemText(hwnd, TXT_LOGIN_USER, usuario, 20);
            GetDlgItemText(hwnd, TXT_LOGIN_PASS, contrasenia, 20);

            Cliente* cliente = buscarClientePorUsuario(raiz, usuario);

            // Verificar si el cliente existe y las credenciales son correctas
            if (cliente != nullptr && strcmp(cliente->usuario.c_str(), usuario) == 0 && strcmp(cliente->contrasenia.c_str(), contrasenia) == 0) {
                usuarioLogueado = cliente->id;
                TipoUsuario = cliente->tipoUsuario;
                clienteComprar = cliente->nombre;

                if (cliente->estatus != "Inactivo") {
                    MessageBox(hwnd, "Inicio de sesión exitoso", "Éxito", MB_OK);
                    EndDialog(hwnd, IDOK);

                    if(TipoUsuario == 1)
                        DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
                    else
                        DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHB_USER), hwnd, fVentanaDashboardUser);
                }
                else {
                    MessageBox(hwnd, "Usuario suspendido", "Error", MB_OK);
                }


            }
            else {
                MessageBox(hwnd, "Usuario o contraseña incorrectos", "Error", MB_OK);
            }
        }
        break;
        case BTN_LOGIN_REGISTER: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTER), hwnd, fVentanaRegister);
        }break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaRegister(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        break;
        case BTN_REGISTRARCLIENTE_CANCELAR: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;
        case BTN_REGISTRARCLIENTE_REGISTER: {
            // Obtener el texto de los cuadros de texto
            
            char nombre[30];
            char apellidoPaterno[30];
            char apellidoMaterno[30];
            char usuario[30];
            char contrasenia[30];

            GetDlgItemText(hwnd, TXT_REGISTRARCLIENTE_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_REGISTRARCLIENTE_AP, apellidoPaterno, sizeof(apellidoPaterno));
            GetDlgItemText(hwnd, TXT_REGISTRARCLIENTE_AM, apellidoMaterno, sizeof(apellidoMaterno));
            GetDlgItemText(hwnd, TXT_REGISTRARCLIENTE_USER, usuario, sizeof(usuario));
            GetDlgItemText(hwnd, TXT_REGISTRARCLIENTE_PASS, contrasenia, sizeof(contrasenia));

            if (strlen(nombre) == 0 || strlen(apellidoPaterno) == 0 || strlen(usuario) == 0 || strlen(contrasenia) == 0) {
                MessageBox(hwnd, "Todos los campos son obligatorios", "Error", MB_OK);
                break;
            }
            ultimoID++;
            raiz = agregarCliente(raiz, ultimoID, nombre, apellidoPaterno, apellidoMaterno, usuario, contrasenia, "Activo", obtenerFechaActual(), 1, 0);

            // Verificar si el cliente fue agregado correctamente
            if (raiz != nullptr) {
                MessageBox(hwnd, "Se ha registrado exitosamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
                EndDialog(hwnd, IDOK);

                DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
            }
            else
                MessageBox(hwnd, "Hubo un error al agregar el cliente.", "Error", MB_OK | MB_ICONERROR);

        }break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaDashboard(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {
        // Cargar y asignar el menú 
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }
        // Obtener el control ListBox desde la ventana
        HWND hwndListBox = GetDlgItem(hwnd, LIS_DASHBOARD_CLIENTES);  
        mostrarClientesEnOrden(raiz, hwndListBox);

        HWND hwndListBoxTiendas = GetDlgItem(hwnd, LIST_DASHBOARD_TIENDAS);
        mostrarTiendasEnListBox(cabezaTienda, hwndListBoxTiendas);

        HWND hwndListBoxProductos = GetDlgItem(hwnd, LIST_DASHBOARD_PROD);
        mostrarProductosEnListBox(cabezaProductos, hwndListBoxProductos);

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;
        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;
        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;
        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;
        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;
        case BTNMENU_SALIR: 
            EndDialog(hwnd, 0);  
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
            break;
        }
        case BTNMENU_REGISTRAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_RC), hwnd, fVentanaRClientes);
            break;
        }
        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }

        }
        
        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaRClientes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {
        // Cargar y asignar el menú (si es necesario y aplicable para un diálogo)
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }
        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;
        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;
        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;
        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;
        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;
        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
            break;
        }

        case BTN_RC_REGISTRAR: {
            // Obtener el texto de los cuadros de texto

            int idUsuarioActualizo = usuarioLogueado;
            char nombre[30];
            char apellidoPaterno[30];
            char apellidoMaterno[30];
            char direccion[30];
            char usuario[30];
            char contrasenia[30];
            int tipoUsuario = -1;

            GetDlgItemText(hwnd, TXT_RC_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_RC_APELLIDOP, apellidoPaterno, sizeof(apellidoPaterno));
            GetDlgItemText(hwnd, TXT_RC_APELLIDOM, apellidoMaterno, sizeof(apellidoMaterno));
            GetDlgItemText(hwnd, TXT_RC_DIRECCION, direccion, sizeof(direccion));
            GetDlgItemText(hwnd, TXT_RC_USUARIO, usuario, sizeof(usuario));
            GetDlgItemText(hwnd, TXT_RC_CONTRA, contrasenia, sizeof(contrasenia));

            // Verificar cuál RadioButton está seleccionado
            if (IsDlgButtonChecked(hwnd, RD_RC_ADMIN) == BST_CHECKED) {
                tipoUsuario = 1; // Administrador
            }
            else if (IsDlgButtonChecked(hwnd, RD_RC_CLIENTE) == BST_CHECKED) {
                tipoUsuario = 0; // Cliente
            }
            else {
                MessageBox(hwnd, "Debe seleccionar un tipo de usuario.", "Error", MB_OK | MB_ICONERROR);
                break;
            }

            if (strlen(nombre) == 0 || strlen(apellidoPaterno) == 0 || strlen(usuario) == 0 || strlen(contrasenia) == 0) {
                MessageBox(hwnd, "Todos los campos son obligatorios", "Error", MB_OK);
                break;
            }
            ultimoID++;
            raiz = agregarCliente(raiz, ultimoID, nombre, apellidoPaterno, apellidoMaterno, usuario, contrasenia, "Activo", obtenerFechaActual(), idUsuarioActualizo, tipoUsuario);

            // Verificar si el cliente fue agregado correctamente
            if (raiz != nullptr) {
                MessageBox(hwnd, "Cliente agregado exitosamente.", "Éxito", MB_OK | MB_ICONINFORMATION);
                EndDialog(hwnd, IDOK);

                // Abrir la ventana de dashboard (suponiendo que es un diálogo)
                DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            }
            else 
                MessageBox(hwnd, "Hubo un error al agregar el cliente.", "Error", MB_OK | MB_ICONERROR);


        }break;
        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaEClientes(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {
        // Cargar y asignar el menú (si es necesario y aplicable para un diálogo)
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }
        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;
        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;
        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;
        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;
        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;
        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }break;
        
        case BTN_EDITAR_BUSCAR: {
            char usuario[20];
            GetDlgItemText(hwnd, TXT_EDITAR_BUSCAR, usuario, sizeof(usuario));

            idClienteEditar = 0;
            Cliente* clienteEditar = buscarClientePorUsuario(raiz, usuario);

            if (clienteEditar != nullptr) {
                idClienteEditar = clienteEditar->id;
                // Colocar la información del cliente en los cuadros de texto
                SetDlgItemText(hwnd, TXT_EDITAR_NOMBRE, clienteEditar->nombre.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_APELLIDOP, clienteEditar->apellidoPaterno.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_APELLIDOM, clienteEditar->apellidoMaterno.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_USUARIO, clienteEditar->usuario.c_str());
                SetDlgItemText(hwnd, TXT_EDITAR_CONTRA, clienteEditar->contrasenia.c_str());

                // Comprobar el estatus y marcar el botón de radio si es "Activo"
                if (clienteEditar->estatus == "Activo") {
                    CheckRadioButton(hwnd, RD_EDITAR_ACTIVO, RD_EDITAR_SUSPENDIDO, RD_EDITAR_ACTIVO);
                }
                else {
                    CheckRadioButton(hwnd, RD_EDITAR_ACTIVO, RD_EDITAR_SUSPENDIDO, RD_EDITAR_SUSPENDIDO);
                }

                // Comprobar el tipo de usuario y marcar el botón de radio si es "Admin"
                if (clienteEditar->tipoUsuario == 1) {
                    CheckRadioButton(hwnd, RD_EC_ADMIN, RD_EC_CLIENTE, RD_EC_ADMIN);
                }
                else {
                    CheckRadioButton(hwnd, RD_EC_ADMIN, RD_EC_CLIENTE, RD_EC_CLIENTE);
                }
            }
            else {
                MessageBox(hwnd, "Cliente no encontrado", "Error", MB_OK | MB_ICONERROR);
            }
        } break;

        case BTN_EDITAR_MODIFICAR: {
            // Obtener el texto de los cuadros de texto
            int idUsuarioActualizo = usuarioLogueado;  // ID del usuario que realiza la actualización
            char nombre[30];
            char apellidoPaterno[30];
            char apellidoMaterno[30];
            char usuario[30];
            char contrasenia[30];
            int tipoUsuario = -1;

            // Obteniendo los textos desde los cuadros de texto de la interfaz
            GetDlgItemText(hwnd, TXT_EDITAR_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_EDITAR_APELLIDOP, apellidoPaterno, sizeof(apellidoPaterno));
            GetDlgItemText(hwnd, TXT_EDITAR_APELLIDOM, apellidoMaterno, sizeof(apellidoMaterno));
            GetDlgItemText(hwnd, TXT_EDITAR_USUARIO, usuario, sizeof(usuario));
            GetDlgItemText(hwnd, TXT_EDITAR_CONTRA, contrasenia, sizeof(contrasenia));

            // Verificar cuál RadioButton está seleccionado
            if (IsDlgButtonChecked(hwnd, RD_EC_ADMIN) == BST_CHECKED) {
                tipoUsuario = 1; // Administrador
            }
            else if (IsDlgButtonChecked(hwnd, RD_EC_CLIENTE) == BST_CHECKED) {
                tipoUsuario = 0; // Cliente
            }
            else {
                MessageBox(hwnd, "Debe seleccionar un tipo de usuario.", "Error", MB_OK | MB_ICONERROR);
                break;
            }

            // Validar que los campos no estén vacíos
            if (strlen(nombre) == 0 || strlen(apellidoPaterno) == 0 || strlen(usuario) == 0 || strlen(contrasenia) == 0) {
                MessageBox(hwnd, "Todos los campos son obligatorios", "Error", MB_OK);
                break;
            }

            // Obtener el estatus seleccionado de los Radio Buttons
            string nuevoEstatus;
            if (IsDlgButtonChecked(hwnd, RD_EDITAR_ACTIVO) == BST_CHECKED) {
                nuevoEstatus = "Activo";
            }
            else if (IsDlgButtonChecked(hwnd, RD_EDITAR_SUSPENDIDO) == BST_CHECKED) {
                nuevoEstatus = "Inactivo";
            }
            else {
                MessageBox(hwnd, "Seleccione un estatus.", "Error", MB_OK);
                break;
            }

            // Convertir los char* a string para llamar a la función editarCliente
            editarCliente(raiz, idClienteEditar, string(nombre), string(apellidoPaterno), string(apellidoMaterno), string(usuario), string(contrasenia), nuevoEstatus, tipoUsuario);
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
        } break;



        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaRTienda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {
        // Cargar y asignar el menú (si es necesario y aplicable para un diálogo)
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }
        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;
        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;
        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;
        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;
        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }break;

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTN_RT_REGISTRAR: {
            
            char nombre[30];
            char direccion[30];

            GetDlgItemText(hwnd, TXT_RT_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_RT_DIREC, direccion, sizeof(direccion));

            if (strlen(nombre) == 0 || strlen(direccion) == 0) {
                MessageBox(hwnd, "Todos los campos son obligatorios", "Error", MB_OK);
                break;
            }
            ultimoIDTienda++;

            agregarTienda(cabezaTienda, ultimoIDTienda, nombre, direccion);
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
        }break;

        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaETienda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {
        // Cargar y asignar el menú (si es necesario y aplicable para un diálogo)
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }

        HWND hwndListBoxTiendas = GetDlgItem(hwnd, LIST_EDITART_TIENDA);
        mostrarTiendasEnListBox(cabezaTienda, hwndListBoxTiendas);
        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        if (HIWORD(wParam) == LBN_SELCHANGE) { // Detectar cambio de selección en el ListBox
            HWND hwndListBox = GetDlgItem(hwnd, LIST_EDITART_TIENDA);
            int index = (int)SendMessage(hwndListBox, LB_GETCURSEL, 0, 0); // Obtener el índice seleccionado

            if (index != LB_ERR) {
                // Recorre la lista de tiendas para encontrar la tienda correspondiente
                Tienda* actual = cabezaTienda;
                int contador = 0;
                while (actual != nullptr && contador < index) {
                    actual = actual->siguiente;
                    contador++;
                }

                if (actual != nullptr) {
                    idTiendaEditar = actual->id;
                    // Rellenar los cuadros de texto con la información de la tienda
                    SetDlgItemText(hwnd, TXT_EDITART_NOMBRE, actual->nombre.c_str());
                    SetDlgItemText(hwnd, TXT_EDITART_DIRECCION, actual->direccion.c_str());
                    SetDlgItemText(hwnd, TXT_EDITART_FECHACT, actual->fechaCambioEstatus.c_str());
                    // Configurar el estado del radio button basado en el estatus
                    if (actual->estatus == "Activa") {
                        CheckRadioButton(hwnd, RB_EDITART_ACTIVO, RB_EDITART_SUSP, RB_EDITART_ACTIVO); // Activa el botón "Activo"
                    }
                    else if (actual->estatus == "Suspendido") {
                        CheckRadioButton(hwnd, RB_EDITART_ACTIVO, RB_EDITART_SUSP, RB_EDITART_SUSP); // Activa el botón "Suspendido"
                    }
                    else {
                        // Opcional: Manejar otros estados si es necesario
                        CheckRadioButton(hwnd, RB_EDITART_ACTIVO, RB_EDITART_SUSP, 0); // Desactiva todos los radio buttons
                    }
                }
            }
        }
        switch (LOWORD(wParam)) {
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;
        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;
        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;
        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }break;

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;
        case BTN_EDITART_MODIFICAR: {
            actualizarTienda(cabezaTienda, idTiendaEditar, hwnd);
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
        }break;

        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaRProductos(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {
        // Cargar y asignar el menú 
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }
        HWND hwndComboBox = GetDlgItem(hwnd, CMB_RP_TIENDAS); // Reemplaza IDC_COMBO_TIENDAS con el ID de tu comboBox

        cargarTiendasEnComboBox(hwndComboBox, cabezaTienda);

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;
        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }break;

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;

        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;

        case BTN_RP_REGISTRAR: {
            char nombre[30];
            char codigo[30];
            char precio[30];
            char costo[30];
            char existencias[30];
            int tiendaID = -1; // Inicializamos con un valor inválido

            GetDlgItemText(hwnd, TXT_RP_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_RP_CODIGO, codigo, sizeof(codigo));
            GetDlgItemText(hwnd, TXT_RP_PRECIO, precio, sizeof(precio));
            GetDlgItemText(hwnd, TXT_RP_COSTO, costo, sizeof(costo));
            GetDlgItemText(hwnd, TXT_RP_EXIS, existencias, sizeof(existencias));

            // Obtener el índice seleccionado en el comboBox
            HWND hwndComboBox = GetDlgItem(hwnd, CMB_RP_TIENDAS); // Reemplaza IDC_COMBO_TIENDAS con el ID de tu comboBox
            int indiceSeleccionado = SendMessage(hwndComboBox, CB_GETCURSEL, 0, 0);

            if (indiceSeleccionado != CB_ERR) {
                // Obtener el ID de la tienda asociado al índice seleccionado
                tiendaID = SendMessage(hwndComboBox, CB_GETITEMDATA, indiceSeleccionado, 0);
            }
            else {
                MessageBox(hwnd, "Por favor selecciona una tienda.", "Error", MB_OK);
                break;
            }
            // Validar que todos los campos obligatorios estén llenos
            if (strlen(nombre) == 0 || strlen(codigo) == 0 || strlen(precio) == 0 || strlen(costo) == 0 || strlen(existencias) == 0) {
                MessageBox(hwnd, "Todos los campos son obligatorios", "Error", MB_OK);
                break;
            }

            // Validar que los campos "precio", "costo" y "existencias" sean números
            if (!esNumero(precio) || !esNumero(costo) || !esNumero(existencias)) {
                MessageBox(hwnd, "Los campos 'precio', 'costo' y 'existencias' deben contener solo números.", "Error", MB_OK);
                break;
            }


            double precioDouble = std::stod(precio);
            double costoDouble = std::stod(costo);
            int existenciasInt = std::stoi(existencias);
            agregarProducto(cabezaProductos, codigo, nombre, precioDouble, costoDouble, existenciasInt, tiendaID);
            MessageBox(nullptr, "Se ha agregado la tienda correctamente", "Info", MB_ICONINFORMATION);

            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard (suponiendo que es un diálogo)
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
        } break;
        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;


        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaReporteCompra(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndListView;
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {

        HWND hwndListView = GetDlgItem(hwnd, LISTVIEW_REPORTE_COMPRAS);

        // Verifica que el control exista
        if (hwndListView) {
            // Configurar estilo como LVS_REPORT (tabla)
            SetWindowLong(hwndListView, GWL_STYLE, GetWindowLong(hwndListView, GWL_STYLE) | LVS_REPORT);

            // Agregar columnas
            LVCOLUMN lvCol = { 0 };
            lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

            // Columna "Producto"
            char colProducto[] = "Producto";
            lvCol.pszText = colProducto;
            lvCol.cx = 100;
            ListView_InsertColumn(hwndListView, 0, &lvCol);

            // Columna "Cantidad"
            char colCantidad[] = "Cantidad";
            lvCol.pszText = colCantidad;
            lvCol.cx = 70;
            ListView_InsertColumn(hwndListView, 1, &lvCol);

            // Columna "Precio Total"
            char colPrecioTotal[] = "Precio Total";
            lvCol.pszText = colPrecioTotal;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 2, &lvCol);

            // Columna "Fecha de compra"
            char colFechaCompra[] = "Fecha compra";
            lvCol.pszText = colFechaCompra;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 3, &lvCol);

            // Columna "Estatus"
            char colEstatus[] = "Estatus";
            lvCol.pszText = colEstatus;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 4, &lvCol);

            // Columna "Tienda"
            char colTienda[] = "Tienda";
            lvCol.pszText = colTienda;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 5, &lvCol);

            // Columna "Cliente"
            char colCliente[] = "Cliente";
            lvCol.pszText = colCliente;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 6, &lvCol);
        }

        if (TipoUsuario == 1) {
            // Cargar y asignar el menú 
            HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
            if (hMenu) {
                SetMenu(hwnd, hMenu);
            }
            cargarComprasEnListView(cabezaCompras, hwndListView);
        }
        else {
            HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU4));
            if (hMenu) {
                SetMenu(hwnd, hMenu);
            }
            cargarComprasEnListViewCliente(cabezaCompras, hwndListView);
        }

        

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case BTNMENU2_MISENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;
        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }break;

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;

        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;

        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;
        case BTNMENU2_INICIO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHB_USER), hwnd, fVentanaDashboardUser);
            break;
        }
        case BTNMENU2_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU2_CERRARSESION: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;


        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaReporteCompraPorTienda(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndListView;
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {

        hwndListView = GetDlgItem(hwnd, LISTVIEW_REPORTE_PORTIENDAS);

        // Verifica que el control exista
        if (hwndListView) {
            // Configurar estilo como LVS_REPORT (tabla)
            SetWindowLong(hwndListView, GWL_STYLE, GetWindowLong(hwndListView, GWL_STYLE) | LVS_REPORT);

            // Agregar columnas
            LVCOLUMN lvCol = { 0 };
            lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

            // Columna "Producto"
            char colProducto[] = "Producto";
            lvCol.pszText = colProducto;
            lvCol.cx = 100;
            ListView_InsertColumn(hwndListView, 0, &lvCol);

            // Columna "Cantidad"
            char colCantidad[] = "Cantidad";
            lvCol.pszText = colCantidad;
            lvCol.cx = 70;
            ListView_InsertColumn(hwndListView, 1, &lvCol);

            // Columna "Precio Total"
            char colPrecioTotal[] = "Precio Total";
            lvCol.pszText = colPrecioTotal;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 2, &lvCol);

            // Columna "Fecha de compra"
            char colFechaCompra[] = "Fecha compra";
            lvCol.pszText = colFechaCompra;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 3, &lvCol);

            // Columna "Estatus"
            char colEstatus[] = "Estatus";
            lvCol.pszText = colEstatus;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 4, &lvCol);

            // Columna "Tienda"
            char colTienda[] = "Tienda";
            lvCol.pszText = colTienda;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 5, &lvCol);

            // Columna "Cliente"
            char colCliente[] = "Cliente";
            lvCol.pszText = colCliente;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 6, &lvCol);
        }

        // Cargar y asignar el menú 
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }
        
        HWND hwndListBoxTiendas = GetDlgItem(hwnd, LIST_REPORTE_PORTIENDA);
        mostrarTiendasEnListBox(cabezaTienda, hwndListBoxTiendas);
       
        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        if (HIWORD(wParam) == LBN_SELCHANGE) { // Detectar cambio de selección en el ListBox
            HWND hwndListBox = GetDlgItem(hwnd, LIST_REPORTE_PORTIENDA);
            int index = (int)SendMessage(hwndListBox, LB_GETCURSEL, 0, 0); // Obtener el índice seleccionado

            ListView_DeleteAllItems(hwndListView);

            if (index != LB_ERR) {
                // Recorre la lista de tiendas para encontrar la tienda correspondiente
                Tienda* actual = cabezaTienda;
                int contador = 0;
                while (actual != nullptr && contador < index) {
                    actual = actual->siguiente;
                    contador++;
                }

                if (actual != nullptr) {
                    //aqui ya la encontro
                    cargarComprasEnListViewTienda(cabezaCompras, hwndListView, actual->id);
                }
            }
        }
        switch (LOWORD(wParam)) {
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }break;

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;

        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;

        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;

        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;


        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaReporteCompraPorProducto(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndListView;
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {

        hwndListView = GetDlgItem(hwnd, LISTVIEW_REPORTE_PORPRODUCTO);

        // Verifica que el control exista
        if (hwndListView) {
            // Configurar estilo como LVS_REPORT (tabla)
            SetWindowLong(hwndListView, GWL_STYLE, GetWindowLong(hwndListView, GWL_STYLE) | LVS_REPORT);

            // Agregar columnas
            LVCOLUMN lvCol = { 0 };
            lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

            // Columna "Producto"
            char colProducto[] = "Producto";
            lvCol.pszText = colProducto;
            lvCol.cx = 100;
            ListView_InsertColumn(hwndListView, 0, &lvCol);

            // Columna "Cantidad"
            char colCantidad[] = "Cantidad";
            lvCol.pszText = colCantidad;
            lvCol.cx = 70;
            ListView_InsertColumn(hwndListView, 1, &lvCol);

            // Columna "Precio Total"
            char colPrecioTotal[] = "Precio Total";
            lvCol.pszText = colPrecioTotal;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 2, &lvCol);

            // Columna "Fecha de compra"
            char colFechaCompra[] = "Fecha compra";
            lvCol.pszText = colFechaCompra;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 3, &lvCol);

            // Columna "Estatus"
            char colEstatus[] = "Estatus";
            lvCol.pszText = colEstatus;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 4, &lvCol);

            // Columna "Tienda"
            char colTienda[] = "Tienda";
            lvCol.pszText = colTienda;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 5, &lvCol);

            // Columna "Cliente"
            char colCliente[] = "Cliente";
            lvCol.pszText = colCliente;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 6, &lvCol);
        }

        // Cargar y asignar el menú 
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }

        HWND hwndListBoxProductos = GetDlgItem(hwnd, LIST_REPORTE_PORPRODUCTO);
        mostrarProductosEnListBox(cabezaProductos, hwndListBoxProductos);

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        if (HIWORD(wParam) == LBN_SELCHANGE) { // Detectar cambio de selección en el ListBox
            HWND hwndListBox = GetDlgItem(hwnd, LIST_REPORTE_PORPRODUCTO);
            int index = (int)SendMessage(hwndListBox, LB_GETCURSEL, 0, 0); // Obtener el índice seleccionado

            ListView_DeleteAllItems(hwndListView);

            if (index != LB_ERR) {
                // Recorre la lista de productos para encontrar la tienda correspondiente
                Producto* actual = cabezaProductos;
                int contador = 0;
                while (actual != nullptr && contador < index) {
                    actual = actual->siguiente;
                    contador++;
                }

                if (actual != nullptr) {
                    //aqui ya la encontro
                    cargarComprasEnListViewProducto(cabezaCompras, hwndListView, actual->codigo);
                }
            }
        }
        switch (LOWORD(wParam)) {
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }break;

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;

        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;

        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;

        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;

        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;


        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaReporteEnvios(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hwndListView;
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {

        idEnvioActualizar = -1;
        hwndListView = GetDlgItem(hwnd, LISTVIEW_ENVIOS);

        // Verifica que el control exista
        if (hwndListView) {
            // Configurar estilo como LVS_REPORT (tabla)
            SetWindowLong(hwndListView, GWL_STYLE, GetWindowLong(hwndListView, GWL_STYLE) | LVS_REPORT);

            // Agregar columnas
            LVCOLUMN lvCol = { 0 };
            lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

            // Columna "Cantidad"
            char colId[] = "Id";
            lvCol.pszText = colId;
            lvCol.cx = 70;
            ListView_InsertColumn(hwndListView, 0, &lvCol);

            // Columna "Producto"
            char colProducto[] = "Producto";
            lvCol.pszText = colProducto;
            lvCol.cx = 100;
            ListView_InsertColumn(hwndListView, 1, &lvCol);

            // Columna "Cantidad"
            char colDireccion[] = "Direccion";
            lvCol.pszText = colDireccion;
            lvCol.cx = 70;
            ListView_InsertColumn(hwndListView, 2, &lvCol);

            // Columna "Estatus"
            char colEstatus[] = "Estatus";
            lvCol.pszText = colEstatus;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 3, &lvCol);

            // Columna "Fecha de compra"
            char colFechaCompra[] = "Fecha actualizacion";
            lvCol.pszText = colFechaCompra;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 4, &lvCol);

            // Columna "Cliente"
            char colCliente[] = "Cliente";
            lvCol.pszText = colCliente;
            lvCol.cx = 90;
            ListView_InsertColumn(hwndListView, 5, &lvCol);
        }


        if (TipoUsuario == 1) {

            HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
            if (hMenu) {
                SetMenu(hwnd, hMenu);
            }

            cargarEnviosEnListView(cabezaEnvios, hwndListView);
        }
        else {

            HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU4));
            if (hMenu) {
                SetMenu(hwnd, hMenu);
            }
            cargarEnviosEnListViewClientes(cabezaEnvios, hwndListView);
        }
        

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        if (HIWORD(wParam) == LBN_SELCHANGE) { // Detectar cambio de selección en el ListBox
            HWND hwndListBox = GetDlgItem(hwnd, LIST_REPORTE_PORPRODUCTO);
            int index = (int)SendMessage(hwndListBox, LB_GETCURSEL, 0, 0); // Obtener el índice seleccionado

            ListView_DeleteAllItems(hwndListView);

            if (index != LB_ERR) {
                // Recorre la lista de productos para encontrar la tienda correspondiente
                Producto* actual = cabezaProductos;
                int contador = 0;
                while (actual != nullptr && contador < index) {
                    actual = actual->siguiente;
                    contador++;
                }

                if (actual != nullptr) {
                    //aqui ya la encontro
                    cargarComprasEnListViewProducto(cabezaCompras, hwndListView, actual->codigo);
                }
            }
        }
        switch (LOWORD(wParam)) {
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }break;

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;

        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;

        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;

        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;

        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTN_ENVIOS_CANCELAR: {
            
            Envios* buscarEnvio = cabezaEnvios;
            while (buscarEnvio != NULL) {

                if (buscarEnvio->id == idEnvioActualizar) {

                }
                buscarEnvio = buscarEnvio->siguiente;
            }
            
            if (!(idEnvioActualizar <= 0)) {

                int respuesta = MessageBox(
                    hwnd,
                    "¿Estás seguro de que deseas cancelar el envío?",
                    "Confirmar Cancelación",
                    MB_YESNO | MB_ICONQUESTION
                );

                if (respuesta == IDYES) {

                    bool encontrado = false;

                    Envios* actual = cabezaEnvios;

                    while (actual != NULL) {
                        if (idEnvioActualizar == actual->id) {
                            if (actual->estatus != "Entregado") {
                                actual->estatus = "Cancelado";
                                encontrado = true;

                                Compra* compraDelCliente = cabezaCompras;
                                while (compraDelCliente != NULL) {
                                    if (compraDelCliente->id == actual->idCompra) {
                                        compraDelCliente->estatus = "Cancelado";
                                        break;
                                    }
                                    compraDelCliente = compraDelCliente->siguiente;
                                }


                                break;
                            }
                            else {
                                MessageBox(hwnd, "Este envio ya se ha entregado", "Advertencia", MB_ICONWARNING);
                            }
                        }
                        actual = actual->siguiente;
                    }

                    if (!encontrado)
                        MessageBox(hwnd, "No se pudo realizar la acción.", "Advertencia", MB_ICONWARNING);
                    else
                        MessageBox(hwnd, "Envío cancelado con éxito", "Info", MB_ICONINFORMATION);

                    EndDialog(hwnd, IDOK);
                    DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
                }
                else {
                    MessageBox(hwnd, "Cancelación abortada.", "Información", MB_ICONINFORMATION);
                }
            }else
                MessageBox(hwnd, "Por favor selecciona un envio", "Éxito", MB_OK | MB_ICONEXCLAMATION);

        }break;
        case BTN_ENVIOS_ENTREGADO: {

            if (!(idEnvioActualizar <= 0)) {

                bool encontrado = false;
                int cantidad = 0;

                Envios* actual = cabezaEnvios;

                while (actual != NULL) {
                    if (idEnvioActualizar == actual->id) {
                        
                        Compra* compraDelCliente = cabezaCompras;
                        while (compraDelCliente != NULL) {
                            if (compraDelCliente->id == actual->idCompra) {
                                cantidad = compraDelCliente->cantidad;
                                compraDelCliente->estatus = "Entregado";
                                break;
                            }
                            compraDelCliente = compraDelCliente->siguiente;
                        }

                        Producto* productoRestar = cabezaProductos;
                        while (productoRestar != NULL) {
                            if (productoRestar->nombre == actual->producto) {
                                productoRestar->existencias = productoRestar->existencias - cantidad;
                                break;
                            }
                            productoRestar = productoRestar->siguiente;
                        }

                        actual->fechaActualizacion = obtenerFechaActual();
                        actual->estatus = "Entregado";
                        encontrado = true;
                        break;
                    }
                    actual = actual->siguiente;
                }

                if (!encontrado)
                    MessageBox(hwnd, "No se encontró un envío con ese ID.", "Advertencia", MB_ICONWARNING);
                else
                    MessageBox(hwnd, "Paquete entregado con éxito", "Info", MB_ICONINFORMATION);

                EndDialog(hwnd, IDOK);
                DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);

            }
            else
                MessageBox(hwnd, "Por favor selecciona un envio", "Éxito", MB_OK | MB_ICONEXCLAMATION);

        }break;
        case BTN_ENVIOS_DEVUELTO: {

            if (!(idEnvioActualizar <= 0)) {

                int respuesta = MessageBox(
                    hwnd,
                    "¿Estás seguro de que deseas devolver el envío?",
                    "Confirmar Devolución",
                    MB_YESNO | MB_ICONQUESTION
                );

                if (respuesta == IDYES) {

                    bool encontrado = false;

                    Envios* actual = cabezaEnvios;

                    while (actual != NULL) {
                        if (idEnvioActualizar == actual->id) {
                            actual->estatus = "Devuelto";
                            encontrado = true;

                            Compra* compraDelCliente = cabezaCompras;
                            while (compraDelCliente != NULL) {
                                if (compraDelCliente->id == actual->idCompra) {
                                    compraDelCliente->estatus = "Devuelto";
                                    break;
                                }
                                compraDelCliente = compraDelCliente->siguiente;
                            }

                            break;
                        }
                        actual = actual->siguiente;
                    }

                    if (!encontrado)
                        MessageBox(hwnd, "No se encontró un envío con ese ID.", "Advertencia", MB_ICONWARNING);
                    else
                        MessageBox(hwnd, "Paquete devuelto con éxito", "Info", MB_ICONINFORMATION);

                    EndDialog(hwnd, IDOK);
                    DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
                }
                else {
                    MessageBox(hwnd, "Devolución cancelada.", "Información", MB_ICONINFORMATION);
                }
            }
            else
                MessageBox(hwnd, "Por favor selecciona un envio", "Éxito", MB_OK | MB_ICONEXCLAMATION);

        }break;

        case BTNMENU2_MISCOMPRAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;
        case BTNMENU2_INICIO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHB_USER), hwnd, fVentanaDashboardUser);
            break;
        }
        case BTNMENU2_SALIR:
            EndDialog(hwnd, 0);
            break;

        case BTNMENU2_CERRARSESION: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        }

        break;

    case WM_NOTIFY: {
        NMHDR* pNMHDR = reinterpret_cast<NMHDR*>(lParam);

        // Verificar si el evento proviene del ListView
        if (pNMHDR->hwndFrom == hwndListView && pNMHDR->code == LVN_ITEMCHANGED) {
            NMLISTVIEW* pNMLV = reinterpret_cast<NMLISTVIEW*>(lParam);

            // Verificar si la fila está seleccionada
            if (pNMLV->uNewState & LVIS_SELECTED) {
                int selectedRow = pNMLV->iItem; // Índice de la fila seleccionada

                // Obtener el texto de la columna "Producto" (asumimos que es la columna 0)
                char producto[200];
                char id[5];
                ListView_GetItemText(hwndListView, selectedRow, 1, producto, sizeof(producto));
                ListView_GetItemText(hwndListView, selectedRow, 0, id, sizeof(id));

                // Establecer el texto en el Label
                SetDlgItemText(hwnd, LBL_ENVIOS_ENVIO, producto);
                SetDlgItemText(hwnd, LBL_ENVIOS_ENVIOID, id);


                idEnvioActualizar = atoi(id);
            }
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaEditarProductos(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {

        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU3));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }

        //listar los productos
        HWND hwndListBoxProductos = GetDlgItem(hwnd, LIST_EDITARPROD_PRODUCTOS);
        mostrarProductosEnListBoxUsuario(cabezaProductos, hwndListBoxProductos);

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        if (HIWORD(wParam) == LBN_SELCHANGE) { // Detectar cambio de selección en el ListBox
            HWND hwndListBox = GetDlgItem(hwnd, LIST_EDITARPROD_PRODUCTOS);
            int index = (int)SendMessage(hwndListBox, LB_GETCURSEL, 0, 0); // Obtener el índice seleccionado

            if (index != LB_ERR) {
                // Recorre la lista de productos para encontrar el producto correspondiente
                Producto* actual = cabezaProductos;
                int contador = 0;
                while (actual != nullptr && contador < index) {
                    actual = actual->siguiente;
                    contador++;
                }

                if (actual != nullptr) {
                    idProductoComprar = actual->codigo;

                    Tienda* tienda = buscarTienda(cabezaTienda, actual->tienda);

                    string existencia = to_string(actual->existencias);
                    int precioEntero = static_cast<int>(actual->precio);
                    double costoEntero = static_cast<int>(actual->costo);
                    string precio = to_string(precioEntero);
                    string costo = to_string(costoEntero);

                    // Rellenar los cuadros de texto con la información del producto
                    SetDlgItemText(hwnd, TXT_EDITARPROD_NOMBRE, actual->nombre.c_str());
                    SetDlgItemText(hwnd, TXT_EDITARPROD_PRECIO, precio.c_str());
                    SetDlgItemText(hwnd, TXT_EDITARPROD_COSTO, costo.c_str());
                    SetDlgItemText(hwnd, TXT_EDITARPROD_EXISTENCIAS, existencia.c_str());
                    SetDlgItemText(hwnd, LBL_EDITARPROD_FECHAACTU, actual->fechaCambioEstatus.c_str());

                    // Activar el radio button correspondiente según el estado del producto
                    if (actual->estatus == "Disponible") {
                        SendDlgItemMessage(hwnd, RB_EDITARPROD_DISPONIBLE, BM_SETCHECK, BST_CHECKED, 0);
                        SendDlgItemMessage(hwnd, RB_EDITARPROD_NODISPONIBLE, BM_SETCHECK, BST_UNCHECKED, 0);
                    }
                    else if (actual->estatus == "NoDisponible") {
                        SendDlgItemMessage(hwnd, RB_EDITARPROD_DISPONIBLE, BM_SETCHECK, BST_UNCHECKED, 0);
                        SendDlgItemMessage(hwnd, RB_EDITARPROD_NODISPONIBLE, BM_SETCHECK, BST_CHECKED, 0);
                    }
                }
            }
        }
        switch (LOWORD(wParam)) {
        case BTNMENU_DASHBOARD: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHBOARD), hwnd, fVentanaDashboard);
            break;
        }
        case BTNMENU_SALIR: // Si tienes un botón de salir o algo similar
            EndDialog(hwnd, 0);  // Cierra la ventana de dashboard
            break;

        case BTNMENU_SALIR_CERRARSESI: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTNMENU_EDITAR_CLIENTES: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_CLIENTE), hwnd, fVentanaEClientes);
        }break;

        case BTNMENU_REGISTRAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_TIENDA), hwnd, fVentanaRTienda);
        }break;

        case BTNMENU_EDITAR_TIENDAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_TIENDA), hwnd, fVentanaETienda);
        }break;

        case BTNMENU_REGISTRAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REGISTRAR_PRODUCTOS), hwnd, fVentanaRProductos);
        }break;

        case BTNMENU_REPORTE_COMPRA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;

        case BTNMENU_REPORTE_PORTIENDA: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORTIENDA), hwnd, fVentanaReporteCompraPorTienda);
        }break;

        case BTNMENU_REPORTE_PORPRODUCTO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTE_PORPRODUCTO), hwnd, fVentanaReporteCompraPorProducto);
        }break;
        case BTNMENU_ENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;
        case BTNMENU_EDITAR_PRODUCTOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_EDITAR_PRODUCTO), hwnd, fVentanaEditarProductos);
        }break;

        case BTN_EDITARPROD_EDITAR: {

            if (idProductoComprar != "null") {
                // Variables para almacenar los valores
                char nombre[100];
                char precio[20];
                char costo[20];
                char existencias[20];
                string estatus;

                // Obtener el texto de los cuadros de texto
                GetDlgItemText(hwnd, TXT_EDITARPROD_NOMBRE, nombre, sizeof(nombre));
                GetDlgItemText(hwnd, TXT_EDITARPROD_PRECIO, precio, sizeof(precio));
                GetDlgItemText(hwnd, TXT_EDITARPROD_COSTO, costo, sizeof(costo));
                GetDlgItemText(hwnd, TXT_EDITARPROD_EXISTENCIAS, existencias, sizeof(existencias));

                if (strlen(nombre) == 0 || strlen(precio) == 0 || strlen(costo) == 0 || strlen(existencias) == 0) {
                    MessageBox(hwnd, "Todos los campos deben ser llenados.", "Error", MB_ICONEXCLAMATION);
                    break;
                }

                // Determinar el estado según los radiobuttons
                if (SendDlgItemMessage(hwnd, RB_EDITARPROD_DISPONIBLE, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    estatus = "Disponible";
                }
                else if (SendDlgItemMessage(hwnd, RB_EDITARPROD_NODISPONIBLE, BM_GETCHECK, 0, 0) == BST_CHECKED) {
                    estatus = "NoDisponible";
                }

                // Conversión de strings a tipos adecuados (si es necesario)
                string nombreProducto = nombre;
                double precioProducto = atof(precio); // Convertir a double
                double costoProducto = atof(costo);  // Convertir a double
                int existenciasProducto = atoi(existencias); // Convertir a entero

                bool seEncontro = false;

                Producto* productoEditar = cabezaProductos;
                while (productoEditar != NULL) {
                    if (productoEditar->codigo == idProductoComprar) {

                        productoEditar->nombre = nombreProducto;
                        productoEditar->precio = precioProducto;
                        productoEditar->costo = costoProducto;
                        productoEditar->existencias = existenciasProducto;
                        productoEditar->estatus = estatus;
                        productoEditar->fechaCambioEstatus = obtenerFechaActual();
                        seEncontro = true;
                        break;
                    }
                    productoEditar = productoEditar->siguiente;
                }

                string mensaje = "Se actualizo el producto exitosamente\nNombre: " + nombreProducto +
                    "\nPrecio: " + to_string(precioProducto) +
                    "\nCosto: " + to_string(costoProducto) +
                    "\nExistencias: " + to_string(existenciasProducto) +
                    "\nEstatus: " + estatus +
                    "\nFecha de actualización: " + obtenerFechaActual();


                if (seEncontro)
                    MessageBox(hwnd, mensaje.c_str(), "Información", MB_ICONINFORMATION);
                else
                    MessageBox(hwnd, "No se encontro el prodoucto", "Información", MB_ICONEXCLAMATION);
            }else
                MessageBox(hwnd, "Debe seleccionar un producto primero", "Información", MB_ICONEXCLAMATION);


        }break;


        }

        break;
    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}


INT_PTR CALLBACK fVentanaDashboardUser(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {
        // Cargar y asignar el menú 
        HMENU hMenu = LoadMenu((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDR_MENU4));
        if (hMenu) {
            SetMenu(hwnd, hMenu);
        }

        //listar los productos
        HWND hwndListBoxProductos = GetDlgItem(hwnd, LIST_DASHUSER_PRODUCTOS);
        mostrarProductosEnListBoxUsuario(cabezaProductos, hwndListBoxProductos);

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:
        
        if (HIWORD(wParam) == LBN_SELCHANGE) { // Detectar cambio de selección en el ListBox
            HWND hwndListBox = GetDlgItem(hwnd, LIST_DASHUSER_PRODUCTOS);
            int index = (int)SendMessage(hwndListBox, LB_GETCURSEL, 0, 0); // Obtener el índice seleccionado

            if (index != LB_ERR) {
                // Recorre la lista de tiendas para encontrar la tienda correspondiente
                Producto* actual = cabezaProductos;
                int contador = 0;
                while (actual != nullptr && contador < index) {
                    actual = actual->siguiente;
                    contador++;
                }

                if (actual != nullptr) {
                    idProductoComprar = actual->codigo;

                    Tienda* tienda = buscarTienda(cabezaTienda, actual->tienda);

                    string existencia = to_string(actual->existencias);
                    int precioEntero = static_cast<int>(actual->precio); // Convertir el precio a entero
                    string precio = to_string(precioEntero);

                    // Rellenar los cuadros de texto con la información del prdocupto
                    SetDlgItemText(hwnd, LBL_DASHUSER_NOMBRE2, actual->nombre.c_str());
                    SetDlgItemText(hwnd, LBL_DASHUSER_PRECIO2, precio.c_str());
                    SetDlgItemText(hwnd, LBL_DASHUSER_EXISTENCIA2, existencia.c_str());
                    SetDlgItemText(hwnd, LBL_DASHUSER_TIENDA2, tienda->nombre.c_str());
                }
            }
        }

        switch (LOWORD(wParam)) {
        case BTNMENU2_MISCOMPRAS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_REPORTECOMPRAS), hwnd, fVentanaReporteCompra);
        }break;
        case BTNMENU2_INICIO: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHB_USER), hwnd, fVentanaDashboardUser);
            break;
        }
        case BTNMENU2_SALIR: 
            EndDialog(hwnd, 0);
            break;

        case BTNMENU2_CERRARSESION: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTN_DASHUSER_COMPRAR: {
            if (idProductoComprar != "null") {
                char tienda[20];

                GetDlgItemText(hwnd, LBL_DASHUSER_TIENDA2, tienda, sizeof(tienda));

                tiendaComprar = tienda;
                EndDialog(hwnd, IDOK);

                DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_COMPRARPRODUCTO), hwnd, fVentanaComprar);
            }
            else {
                MessageBox(nullptr, "Selecciona un producto primero", "Info", MB_ICONEXCLAMATION);
            }

        }break;
        case BTNMENU2_MISENVIOS: {
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_ENVIOS), hwnd, fVentanaReporteEnvios);
        }break;


        
        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

INT_PTR CALLBACK fVentanaComprar(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_INITDIALOG: {

        SYSTEMTIME tiempo;
        GetLocalTime(&tiempo); // Obtiene la fecha y hora actual del sistema

        // Formatear la fecha como "DD/MM/YYYY"
        ostringstream fecha;
        fecha << tiempo.wDay + 1<< "/"
            << tiempo.wMonth << "/"
            << tiempo.wYear;
        string fechaApp = fecha.str();

        fechaAprox = fechaApp;

        SetDlgItemText(hwnd, LBL_COMPP_FECHA2, fechaApp.c_str());
        
        Producto* aux = cabezaProductos;

        while (aux != NULL) {

            if (aux->codigo == idProductoComprar) {
                // Convertir el nombre a una cadena de caracteres (si es necesario)
                const char* nombreProducto = aux->nombre.c_str();

                precioUnitario = aux->precio;

                // Convertir el precio a cadena con formato adecuado
                char precioProducto[20]; // Buffer suficiente para el precio formateado
                snprintf(precioProducto, sizeof(precioProducto), "%.2f", aux->precio);

                idTiendaComprar = aux->tienda;
                // Enviar los valores a los labels
                SetDlgItemText(hwnd, LBL_COMPP_PRODUCTO, nombreProducto);
                SetDlgItemText(hwnd, LBL_COMPP_PU, precioProducto);
                SetDlgItemText(hwnd, LBL_COMPP_PT, precioProducto);
                SetDlgItemText(hwnd, TXT_COMPP_CANTIDAD, "1");

                break; // Salir del bucle una vez encontrado el producto
            }
            aux = aux->siguiente;
        }

        return TRUE; // Devuelve TRUE para indicar que se ha inicializado correctamente
    }

    case WM_COMMAND:

        switch (LOWORD(wParam)) {
        case BTNMENU2_INICIO: {
            EndDialog(hwnd, IDOK);

            // Abrir la ventana de dashboard de usuario
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHB_USER), hwnd, fVentanaDashboardUser);
            break;
        }
        case BTNMENU2_SALIR: 
            EndDialog(hwnd, 0);
            break;

        case BTNMENU2_CERRARSESION: {
            usuarioLogueado = 0;
            TipoUsuario = -1;
            EndDialog(hwnd, IDOK);

            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_LOGIN), hwnd, fVentanaLogin);
        }break;

        case BTN_COMPP_CANCELAR: {
            idProductoComprar = "null";
            EndDialog(hwnd, IDOK);
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHB_USER), hwnd, fVentanaDashboardUser);
            break;
        }break;
        case TXT_COMPP_CANTIDAD: {
            if (HIWORD(wParam) == EN_CHANGE) {
                // Leer el texto del textbox
                char buffer[10];
                GetDlgItemText(hwnd, TXT_COMPP_CANTIDAD, buffer, sizeof(buffer));
               
                // Convertir el texto ingresado a número
                int cantidad = 0;
                try {
                    cantidad = stoi(buffer); // Convertir a entero
                }
                catch (...) {
                    cantidad = 0; // En caso de error, cantidad será 0
                }

                // Calcular el total
                double total = cantidad * precioUnitario;
                
                // Formatear el total con dos decimales
                char totalTexto[20];
                snprintf(totalTexto, sizeof(totalTexto), "%.2f", total);

                // Actualizar el label con el total
                SetDlgItemText(hwnd, LBL_COMPP_PT, totalTexto);
            }
        }break;

        case BTN_COMPP_COMPRAR: {
            char producto[30], direccion[30], cantidadStr[30], precioUStr[30], precioTStr[30];

            // Obtener los datos de los controles
            GetDlgItemText(hwnd, LBL_COMPP_PRODUCTO, producto, sizeof(producto));
            GetDlgItemText(hwnd, TXT_COMPP_DIRECCION, direccion, sizeof(direccion));
            GetDlgItemText(hwnd, TXT_COMPP_CANTIDAD, cantidadStr, sizeof(cantidadStr));
            GetDlgItemText(hwnd, LBL_COMPP_PU, precioUStr, sizeof(precioUStr));
            GetDlgItemText(hwnd, LBL_COMPP_PT, precioTStr, sizeof(precioTStr));

            // Convertir las cadenas a sus respectivos tipos
            int cantidad = 0;
            double precioUnitario = 0.0;
            try {
                cantidad = stoi(cantidadStr);          // Convertir cantidad a entero
                precioUnitario = stod(precioUStr);    // Convertir precio unitario a double
            }
            catch (...) {
                MessageBox(hwnd, "Datos inválidos. Verifique la cantidad y precio.", "Error", MB_OK | MB_ICONERROR);
                break;
            }
            
            bool maximoDeCantidad = false;

            //verificacion si selecciono mas de las existencias
            Producto* productoBuscar = cabezaProductos;
            while (productoBuscar != NULL) {
                if (productoBuscar->codigo == idProductoComprar) {
                    if (cantidad > productoBuscar->existencias) {
                        MessageBox(hwnd, "No hay suficientes productos.", "Error", MB_OK | MB_ICONERROR);
                        maximoDeCantidad = true;
                        break;
                    }
                    if (productoBuscar->estatus == "NoDisponible") {
                        MessageBox(hwnd, "Este producto no esta disponible", "Error", MB_OK | MB_ICONERROR);
                        maximoDeCantidad = true;
                        break;
                    }
                }
                productoBuscar = productoBuscar->siguiente;
            }

            if (maximoDeCantidad)
                break;

            // Verificar datos obligatorios
            if (strlen(producto) == 0 || strlen(direccion) == 0 || cantidad <= 0 || precioUnitario <= 0.0) {
                MessageBox(hwnd, "Todos los campos son obligatorios y deben ser válidos.", "Error", MB_OK | MB_ICONERROR);
                break;
            }
            ultimoIdCompra++;
            // Preparar la cadena para el MessageBox
            stringstream msg;
            msg << "Compra registrada con éxito." "\n"
                << "ID: " << ultimoIdCompra << "\n"
                << "Tienda: " << tiendaComprar << "\n"
                << "Producto: " << producto << "\n"
                << "Dirección: " << direccion << "\n"
                << "Cantidad: " << cantidad << "\n"
                << "Precio Unitario: $" << precioUnitario << "\n"
                << "Precio Total: $" << (cantidad * precioUnitario) << "\n"
                << "Fecha Aproximada: " << fechaAprox << "\n"
                << "Cliente: " << clienteComprar;

            stringstream msg2;
            msg2 << "Se ha enviado su paquete." "\n"
                << "Producto: " << producto << "\n"
                << "con dirección: " << direccion << "\n\n\n"
                << "Gracias por su preferencia :D ";

            // Agregar la compra a la lista
            agregarCompra(cabezaCompras, ultimoIdCompra, producto, direccion, cantidad, precioUnitario, fechaAprox, clienteComprar, tiendaComprar,
                idTiendaComprar, usuarioLogueado, idProductoComprar);

            MessageBox(hwnd, msg.str().c_str(), "Información", MB_OK | MB_ICONINFORMATION);

            ultimoIdEnvio++;
            agregarEnvio(cabezaEnvios, ultimoIdEnvio, ultimoIdCompra, producto, direccion, clienteComprar);
            MessageBox(hwnd, msg2.str().c_str(), "Enviado", MB_ICONINFORMATION);

            idProductoComprar = "null";
            EndDialog(hwnd, IDOK);
            DialogBox((HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(DLG_DASHB_USER), hwnd, fVentanaDashboardUser);

        }break;

        }

        break;

    case WM_DESTROY:
        PostQuitMessage(9);
        break;
    }
    return FALSE;
}

//================================================================== FUNCIONES =====================================================================


//------------------------------------Gestion de Usuarios------------------------------------------------------------------
// Función para obtener la fecha actual
string obtenerFechaActual() {
    time_t t = time(0);
    struct tm* now = localtime(&t);
    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d", now);
    return buffer;
}

// Función para agregar un cliente al árbol
Cliente* agregarCliente(Cliente* raiz, int id, const string& nombre, const string& apellidoPaterno, const string& apellidoMaterno, const string& usuario, const string& contrasenia, const string& estatus, const string& fechaActualizo, int idUsuarioActualizo, int tipoUsuario) {
    if (raiz == nullptr) {
        return new Cliente(id, nombre, apellidoPaterno, apellidoMaterno, usuario, contrasenia, estatus, fechaActualizo, idUsuarioActualizo, tipoUsuario);
    }

    if (id < raiz->id) {
        raiz->izquierda = agregarCliente(raiz->izquierda, id, nombre, apellidoPaterno, apellidoMaterno, usuario, contrasenia, estatus, fechaActualizo, idUsuarioActualizo, tipoUsuario);
    }

    else if (id > raiz->id) {
        raiz->derecha = agregarCliente(raiz->derecha, id, nombre, apellidoPaterno, apellidoMaterno, usuario, contrasenia, estatus, fechaActualizo, idUsuarioActualizo, tipoUsuario);
    }

    else {
        return raiz; // Puedes retornar aquí para evitar sobreescribir nada más
    }

    return raiz;
}

// Función para mostrar los clientes en orden en un ListBox (solo nombre, apellido paterno y apellido materno)
void mostrarClientesEnOrden(Cliente* raiz, HWND hwndListBox) {
    if (raiz == nullptr) return;  // Si el árbol está vacío, no hace nada

    // Llamada recursiva para recorrer el subárbol izquierdo
    mostrarClientesEnOrden(raiz->izquierda, hwndListBox);

    // Crear el texto que se va a mostrar en el ListBox
    string clienteInfo = to_string(raiz->id) + " - " + raiz->nombre + " " + raiz->apellidoPaterno;

    // Convertir `clienteInfo` a un `const char*` para usarlo en WinAPI
    SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)clienteInfo.c_str());

    // Llamada recursiva para recorrer el subárbol derecho
    mostrarClientesEnOrden(raiz->derecha, hwndListBox);
}

// Función para cambiar el estatus de un // Función para editar los detalles de un cliente
void editarCliente(Cliente* raiz, int id, const string& nuevoNombre, const string& nuevoApellidoPaterno, const string& nuevoApellidoMaterno, const string& nuevoUsuario, const string& nuevaContrasenia, const string& nuevoEstatus, int tipoUsuario) {
    Cliente* cliente = busquedaBinariaCliente(raiz, id);
    if (cliente == nullptr) {
        MessageBox(nullptr, "No se encontro el cliente", "Error", MB_ICONERROR);
        return;
    }

    // Actualizar los detalles del cliente
    cliente->nombre = nuevoNombre;
    cliente->apellidoPaterno = nuevoApellidoPaterno;
    cliente->apellidoMaterno = nuevoApellidoMaterno;
    cliente->usuario = nuevoUsuario;
    cliente->contrasenia = nuevaContrasenia;
    cliente->estatus = nuevoEstatus;
    cliente->fechaCambioEstatus = obtenerFechaActual();
    cliente->idUsuarioActualizo = usuarioLogueado;
    cliente->tipoUsuario = tipoUsuario;

    MessageBox(nullptr, "Cliente editado exitosamente", "Info", MB_ICONINFORMATION);
}

// Función para liberar la memoria del árbol
void eliminarArbolClientes(Cliente* raiz) {
    if (raiz == nullptr) return;
    eliminarArbolClientes(raiz->izquierda);  // Liberar subárbol izquierdo
    eliminarArbolClientes(raiz->derecha);    // Liberar subárbol derecho
    delete raiz;                     // Eliminar el nodo actual
}

// Función para guardar los datos en un archivo binario
void guardarDatos(Cliente* raiz, ofstream& archivo) {
    if (raiz == nullptr) return;

    // Guardar el ID del cliente
    archivo.write(reinterpret_cast<char*>(&raiz->id), sizeof(raiz->id));

    // Guardar el nombre del cliente
    size_t nombreSize = raiz->nombre.size();
    archivo.write(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize));
    archivo.write(raiz->nombre.c_str(), nombreSize);

    // Guardar el nombre del cliente
    size_t apellidoPaternoSize = raiz->apellidoPaterno.size();
    archivo.write(reinterpret_cast<char*>(&apellidoPaternoSize), sizeof(apellidoPaternoSize));
    archivo.write(raiz->apellidoPaterno.c_str(), apellidoPaternoSize);

    // Guardar el nombre del cliente
    size_t apellidoMaternoSize = raiz->apellidoMaterno.size();
    archivo.write(reinterpret_cast<char*>(&apellidoMaternoSize), sizeof(apellidoMaternoSize));
    archivo.write(raiz->apellidoMaterno.c_str(), apellidoMaternoSize);

    // Guardar el nombre del cliente
    size_t usuarioSize = raiz->usuario.size();
    archivo.write(reinterpret_cast<char*>(&usuarioSize), sizeof(usuarioSize));
    archivo.write(raiz->usuario.c_str(), usuarioSize);

    // Guardar el nombre del cliente
    size_t contraSize = raiz->contrasenia.size();
    archivo.write(reinterpret_cast<char*>(&contraSize), sizeof(contraSize));
    archivo.write(raiz->contrasenia.c_str(), contraSize);

    // Guardar el estatus del cliente
    size_t estatusSize = raiz->estatus.size();
    archivo.write(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
    archivo.write(raiz->estatus.c_str(), estatusSize);

    // Guardar la fecha de registro
    archivo.write(raiz->fechaRegistro.c_str(), raiz->fechaRegistro.size());
    archivo.put('\0');  // Añadir el terminador nulo

    // Guardar la fecha de cambio de estatus
    archivo.write(raiz->fechaCambioEstatus.c_str(), raiz->fechaCambioEstatus.size());
    archivo.put('\0');  // Añadir el terminador nulo

    archivo.write(reinterpret_cast<char*>(&raiz->idUsuarioActualizo), sizeof(raiz->idUsuarioActualizo));

    // Guardar el tipo de usuario
    archivo.write(reinterpret_cast<char*>(&raiz->tipoUsuario), sizeof(raiz->tipoUsuario));

    // Recursión para guardar los subárboles izquierdo y derecho
    guardarDatos(raiz->izquierda, archivo);
    guardarDatos(raiz->derecha, archivo);
}

Cliente* cargarDatos(ifstream& archivo, Cliente* raiz) {
    int id;

    // Intenta leer el ID del cliente
    if (!archivo.read(reinterpret_cast<char*>(&id), sizeof(id))) {
        return raiz;  // Retorna si no se puede leer más (final del archivo)
    }
    if (id > ultimoID) {
        ultimoID = id;
    }
    // Crea un nuevo cliente y asigna su ID
    Cliente* nuevoCliente = new Cliente();
    nuevoCliente->id = id;

    // Leer el nombre del cliente
    size_t nombreSize;
    if (!archivo.read(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->nombre.resize(nombreSize);
    if (!archivo.read(&nuevoCliente->nombre[0], nombreSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el apellido paterno del cliente
    size_t apellidoPaternoSize;
    if (!archivo.read(reinterpret_cast<char*>(&apellidoPaternoSize), sizeof(apellidoPaternoSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->apellidoPaterno.resize(apellidoPaternoSize);
    if (!archivo.read(&nuevoCliente->apellidoPaterno[0], apellidoPaternoSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el apellido materno del cliente
    size_t apellidoMaternoSize;
    if (!archivo.read(reinterpret_cast<char*>(&apellidoMaternoSize), sizeof(apellidoMaternoSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->apellidoMaterno.resize(apellidoMaternoSize);
    if (!archivo.read(&nuevoCliente->apellidoMaterno[0], apellidoMaternoSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el usuario del cliente
    size_t usuarioSize;
    if (!archivo.read(reinterpret_cast<char*>(&usuarioSize), sizeof(usuarioSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->usuario.resize(usuarioSize);
    if (!archivo.read(&nuevoCliente->usuario[0], usuarioSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el contrasenia del cliente
    size_t contraSize;
    if (!archivo.read(reinterpret_cast<char*>(&contraSize), sizeof(contraSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->contrasenia.resize(contraSize);
    if (!archivo.read(&nuevoCliente->contrasenia[0], contraSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el estatus del cliente
    size_t estatusSize;
    if (!archivo.read(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }
    nuevoCliente->estatus.resize(estatusSize);
    if (!archivo.read(&nuevoCliente->estatus[0], estatusSize)) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer fechas
    getline(archivo, nuevoCliente->fechaRegistro, '\0');
    getline(archivo, nuevoCliente->fechaCambioEstatus, '\0');

    // Leer el idUsuarioActualizo
    if (!archivo.read(reinterpret_cast<char*>(&nuevoCliente->idUsuarioActualizo), sizeof(nuevoCliente->idUsuarioActualizo))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Leer el tipo de usuario
    if (!archivo.read(reinterpret_cast<char*>(&nuevoCliente->tipoUsuario), sizeof(nuevoCliente->tipoUsuario))) {
        delete nuevoCliente; // Limpia la memoria si hay error
        return nullptr;
    }

    // Agregar cliente al árbol
    raiz = agregarCliente(raiz, nuevoCliente->id, nuevoCliente->nombre, nuevoCliente->apellidoPaterno, nuevoCliente->apellidoMaterno, nuevoCliente->usuario, nuevoCliente->contrasenia, nuevoCliente->estatus, nuevoCliente->fechaCambioEstatus, nuevoCliente->idUsuarioActualizo, nuevoCliente->tipoUsuario);

    // Leer los subárboles izquierdo y derecho
    raiz = cargarDatos(archivo, raiz); // Asegúrate de que esto mantenga la referencia correcta a la raíz

    return raiz;  // Retorna la raíz del árbol actualizado
}

Cliente* buscarClientePorID(Cliente* raiz, int clienteID) {
    if (raiz == nullptr) {
        return nullptr;  // Si el árbol está vacío, no hay cliente
    }

    if (raiz->id == clienteID) {
        return raiz;  // Si el ID coincide, se encontró el cliente
    }
    else if (clienteID < raiz->id) {
        return buscarClientePorID(raiz->izquierda, clienteID);  // Búsqueda en el subárbol izquierdo
    }
    else {
        return buscarClientePorID(raiz->derecha, clienteID);  // Búsqueda en el subárbol derecho
    }
}

Cliente* buscarClientePorUsuario(Cliente* raiz, const string& usuario) {
    // Caso base: Si el árbol está vacío, no hay cliente que buscar
    if (raiz == nullptr) {
        return nullptr;
    }

    // Si encontramos el cliente con el usuario solicitado, lo regresamos
    if (raiz->usuario == usuario) {
        return raiz;
    }

    // Si no lo encontramos, seguimos buscando en los subárboles
    // Primero en el subárbol izquierdo
    Cliente* resultado = buscarClientePorUsuario(raiz->izquierda, usuario);

    // Si no lo encontramos en el subárbol izquierdo, buscamos en el subárbol derecho
    if (resultado == nullptr) {
        resultado = buscarClientePorUsuario(raiz->derecha, usuario);
    }

    return resultado;
}


//------------------------------------Gestion de TIENDA------------------------------------------------------------------

// 1. Función para agregar una nueva tienda a la lista doblemente ligada
void agregarTienda(Tienda*& cabeza, int id, const string& nombre, const string& direccion) {
    // Validar que no exista un ID duplicado
    Tienda* actual = cabeza;
    while (actual != nullptr) {
        if (actual->id == id) {
            MessageBox(nullptr, "El ID esta repetido", "Error", MB_ICONERROR);
            return; // Salir si se encuentra un ID duplicado
        }
        actual = actual->siguiente;
    }

    // Crear una nueva tienda
    Tienda* nuevaTienda = new Tienda(id, nombre, direccion);

    if (cabeza == nullptr) {
        // Si la lista está vacía, la nueva tienda será la cabeza
        cabeza = nuevaTienda;
    }
    else {
        // Insertar al final de la lista
        actual = cabeza;
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevaTienda;
        nuevaTienda->anterior = actual;
    }
    MessageBox(nullptr, "Se ha agregado la tienda correctamente", "Info", MB_ICONINFORMATION);
}

void guardarTiendas(Tienda* cabeza, const string& nombreArchivo) {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo de TIENDA", "Error", MB_ICONERROR);
        return;
    }

    Tienda* actual = cabeza;
    while (actual != nullptr) {
        archivo.write(reinterpret_cast<char*>(&actual->id), sizeof(actual->id));
        size_t nombreSize = actual->nombre.size();
        archivo.write(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize));
        archivo.write(actual->nombre.c_str(), nombreSize);

        size_t direccionSize = actual->direccion.size();
        archivo.write(reinterpret_cast<char*>(&direccionSize), sizeof(direccionSize));
        archivo.write(actual->direccion.c_str(), direccionSize);

        size_t estatusSize = actual->estatus.size();
        archivo.write(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
        archivo.write(actual->estatus.c_str(), estatusSize);

        // Aquí puedes guardar la fecha de cambio de estatus
        size_t fechaCambioSize = actual->fechaCambioEstatus.size();
        archivo.write(reinterpret_cast<char*>(&fechaCambioSize), sizeof(fechaCambioSize));
        archivo.write(actual->fechaCambioEstatus.c_str(), fechaCambioSize);

        actual = actual->siguiente; // Mover al siguiente nodo
    }

    archivo.close();
}

// Función para cargar tiendas desde un archivo binario
Tienda* cargarTiendas(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo TIENDAS", "Error", MB_ICONERROR);
        return nullptr;
    }

    Tienda* cabeza = nullptr;
    Tienda* cola = nullptr; // Para mantener el último nodo
    while (true) {
        Tienda* nuevaTienda = new Tienda(0, "", ""); // Crear un nuevo nodo temporal

        archivo.read(reinterpret_cast<char*>(&nuevaTienda->id), sizeof(nuevaTienda->id));
        if (archivo.eof()) {
            delete nuevaTienda; // Eliminar nodo temporal si se llegó al final
            break;
        }

        // Actualizar el último ID si el actual es mayor
        if (nuevaTienda->id > ultimoIDTienda) {
            ultimoIDTienda = nuevaTienda->id;
        }

        // Cargar nombre
        size_t nombreSize;
        archivo.read(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize));
        nuevaTienda->nombre.resize(nombreSize);
        archivo.read(&nuevaTienda->nombre[0], nombreSize);

        // Cargar direccion
        size_t direccionSize;
        archivo.read(reinterpret_cast<char*>(&direccionSize), sizeof(direccionSize));
        nuevaTienda->direccion.resize(direccionSize);
        archivo.read(&nuevaTienda->direccion[0], direccionSize);

        // Cargar estatus
        size_t estatusSize;
        archivo.read(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
        nuevaTienda->estatus.resize(estatusSize);
        archivo.read(&nuevaTienda->estatus[0], estatusSize);

        // Cargar fecha de cambio de estatus
        size_t fechaCambioSize;
        archivo.read(reinterpret_cast<char*>(&fechaCambioSize), sizeof(fechaCambioSize));
        nuevaTienda->fechaCambioEstatus.resize(fechaCambioSize);
        archivo.read(&nuevaTienda->fechaCambioEstatus[0], fechaCambioSize);

        // Enlazar en la lista
        nuevaTienda->siguiente = nullptr;
        nuevaTienda->anterior = cola; // Establecer puntero anterior
        if (cola != nullptr) {
            cola->siguiente = nuevaTienda; // Conectar el nodo anterior
        }
        else {
            cabeza = nuevaTienda; // Si es el primer nodo, establecer cabeza
        }
        cola = nuevaTienda; // Actualizar cola
    }

    archivo.close();
    return cabeza; // Retornar la cabeza de la lista
}

void eliminarTiendas(Tienda*& cabeza) {
    while (cabeza != nullptr) {
        Tienda* temp = cabeza;
        cabeza = cabeza->siguiente; // Mover la cabeza al siguiente nodo
        delete temp; // Liberar el nodo
    }
}

// Función para mostrar las tiendas en un ListBox (solo el nombre)
void mostrarTiendasEnListBox(Tienda* cabeza, HWND hwndListBox) {
    if (cabeza == nullptr) {
        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)"No hay tiendas");
        MessageBox(nullptr, "No hay tiendas para mostrar.", "Información", MB_ICONINFORMATION);
        return;
    }

    // Limpiar el ListBox antes de agregar nuevos elementos
    SendMessage(hwndListBox, LB_RESETCONTENT, 0, 0);

    Tienda* actual = cabeza;
    while (actual != nullptr) {
        // Formatear la cadena para mostrar el ID y el nombre de la tienda
        std::string tiendaInfo = "ID: " + std::to_string(actual->id) + " - " + actual->nombre;
        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)tiendaInfo.c_str());
        actual = actual->siguiente;
    }
}

void actualizarTienda(Tienda*& cabeza, int idTiendaEditar, HWND hwnd) {
    Tienda* tienda = cabeza;

    // Buscar la tienda con el ID proporcionado
    while (tienda != nullptr) {
        if (tienda->id == idTiendaEditar) {
            // Obtener los valores de los cuadros de texto
            char nombre[30], direccion[30];
            GetDlgItemText(hwnd, TXT_EDITART_NOMBRE, nombre, sizeof(nombre));
            GetDlgItemText(hwnd, TXT_EDITART_DIRECCION, direccion, sizeof(direccion));

            // Obtener el estatus del radio button
            bool estatusActivo = IsDlgButtonChecked(hwnd, RB_EDITART_ACTIVO);
            bool estatusSuspendido = IsDlgButtonChecked(hwnd, RB_EDITART_SUSP);

            // Verificar que los campos obligatorios no estén vacíos
            if (strlen(nombre) == 0 || strlen(direccion) == 0) {
                MessageBox(hwnd, "El nombre y la dirección son obligatorios", "Error", MB_OK | MB_ICONERROR);
                return;
            }

            // Actualizar los datos de la tienda
            tienda->nombre = nombre;
            tienda->direccion = direccion;

            // Actualizar el estatus según el radio button seleccionado
            if (estatusActivo) {
                tienda->estatus = "Activa";
            }
            else if (estatusSuspendido) {
                tienda->estatus = "Suspendido";
            }
            else {
                MessageBox(hwnd, "Debe seleccionar un estatus", "Error", MB_OK | MB_ICONERROR);
                return;
            }

            // Fecha de cambio de estatus (opcional, si quieres registrar cuándo se hizo la actualización)
            tienda->fechaCambioEstatus = obtenerFechaActual(); // Asumiendo que tienes una función para obtener la fecha

            // Mostrar mensaje de éxito
            MessageBox(hwnd, "La tienda se ha actualizado correctamente", "Éxito", MB_OK | MB_ICONINFORMATION);
            return;
        }

        tienda = tienda->siguiente; // Continuar con la siguiente tienda
    }

    // Si no se encuentra la tienda
    MessageBox(hwnd, "Tienda no encontrada", "Error", MB_OK | MB_ICONERROR);
}

// Función para cargar las tiendas en el comboBox
void cargarTiendasEnComboBox(HWND hwndComboBox, Tienda* cabezaTiendas) {
    // Limpiar el ComboBox antes de agregar nuevos elementos
    SendMessage(hwndComboBox, CB_RESETCONTENT, 0, 0);

    Tienda* actual = cabezaTiendas;
    while (actual != nullptr) {
        // Agregar el nombre de la tienda al ComboBox
        int indice = SendMessage(hwndComboBox, CB_ADDSTRING, 0, (LPARAM)actual->nombre.c_str());

        // Asociar el ID de la tienda con el elemento recién añadido
        SendMessage(hwndComboBox, CB_SETITEMDATA, indice, (LPARAM)actual->id);

        actual = actual->siguiente;
    }
}


int obtenerIdTiendaPorIndice(HWND hwndComboBox, Tienda* cabezaTiendas) {
    int indiceSeleccionado = SendMessage(hwndComboBox, CB_GETCURSEL, 0, 0);  // Obtener el índice seleccionado

    if (indiceSeleccionado == CB_ERR) {
        // Si no hay selección válida, retornar un valor inválido
        return -1;
    }

    // Asumimos que el índice corresponde al orden de las tiendas
    Tienda* actual = cabezaTiendas;
    int indiceActual = 0;

    while (actual != nullptr) {
        if (indiceActual == indiceSeleccionado) {
            // Devolver el ID de la tienda directamente
            return actual->id;
        }
        actual = actual->siguiente;
        indiceActual++;
    }

    return -1;
}

Tienda* buscarTienda(Tienda* inicio, int id) {
    Tienda* actual = inicio;
    while (actual != nullptr) {
        if (actual->id == id) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return nullptr; // No encontrada
}



//------------------------------------Gestion de PRODUCTOS------------------------------------------------------------------

void agregarProducto(Producto*& cabeza, const string& codigo, const string& nombre, double precio, double costo, int existencias, int tienda) {
    if (existencias < 0) {
        MessageBox(nullptr, "No se puede agregar el producto. Las existencias no pueden ser negativas.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Verificar si el código ya existe
    Producto* actual = cabeza;
    while (actual != nullptr) {
        if (actual->codigo == codigo) {
            MessageBox(nullptr, "No se puede agregar el producto. Ya existe un producto con el mismo código.", "Error", MB_OK | MB_ICONERROR);
            return;
        }
        actual = actual->siguiente;
    }

    // Crear el nuevo producto
    Producto* nuevoProducto = new Producto(codigo, nombre, precio, costo, existencias, tienda);

    if (cabeza == nullptr) {
        // Si la lista está vacía, la nueva tienda será la cabeza
        cabeza = nuevoProducto;
    }
    else {
        // Insertar al final de la lista
        actual = cabeza;
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevoProducto;
        nuevoProducto->anterior = actual;
    }
}

void liberarProductos(Producto*& cabeza) {
    Producto* actual = cabeza;
    while (actual != nullptr) {
        Producto* siguiente = actual->siguiente; // Guardamos el siguiente nodo
        delete actual; // Liberamos la memoria del nodo actual
        actual = siguiente; // Avanzamos al siguiente nodo
    }
    cabeza = nullptr; // Aseguramos que la cabeza apunte a nullptr después de liberar
}

void guardarProductos(Producto* cabeza, const string& nombreArchivo) {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo para guardar productos.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    Producto* actual = cabeza;
    while (actual != nullptr) {
        // Guardamos el código del producto
        size_t codigoSize = actual->codigo.size();
        archivo.write(reinterpret_cast<char*>(&codigoSize), sizeof(codigoSize));
        archivo.write(actual->codigo.c_str(), codigoSize);

        // Guardamos el nombre del producto
        size_t nombreSize = actual->nombre.size();
        archivo.write(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize));
        archivo.write(actual->nombre.c_str(), nombreSize);

        // Guardamos el precio y costo
        archivo.write(reinterpret_cast<char*>(&actual->precio), sizeof(actual->precio));
        archivo.write(reinterpret_cast<char*>(&actual->costo), sizeof(actual->costo));

        // Guardamos las existencias
        archivo.write(reinterpret_cast<char*>(&actual->existencias), sizeof(actual->existencias));

        // Guardamos el id de la tienda
        archivo.write(reinterpret_cast<char*>(&actual->tienda), sizeof(actual->tienda));

        // Guardamos el estatus
        size_t estatusSize = actual->estatus.size();
        archivo.write(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
        archivo.write(actual->estatus.c_str(), estatusSize);

        // Guardamos la fecha de cambio de estatus
        size_t fechaCambioEstatusSize = actual->fechaCambioEstatus.size();
        archivo.write(reinterpret_cast<char*>(&fechaCambioEstatusSize), sizeof(fechaCambioEstatusSize));
        archivo.write(actual->fechaCambioEstatus.c_str(), fechaCambioEstatusSize);

        // Avanzamos al siguiente producto
        actual = actual->siguiente;
    }

    archivo.close();
   
}


Producto* cargarProductos(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo de productos para cargar.", "Error", MB_OK | MB_ICONERROR);
        return nullptr;
    }

    Producto* cabeza = nullptr;
    Producto* cola = nullptr; // Para mantener el último nodo

    while (true) {
        // Creamos un nodo temporal para el producto
        Producto* nuevoProducto = nullptr;

        // Leemos el tamaño y código del producto
        size_t codigoSize;
        archivo.read(reinterpret_cast<char*>(&codigoSize), sizeof(codigoSize));
        if (archivo.eof()) break; // Si alcanzamos el final, terminamos la carga

        string codigo(codigoSize, '\0');
        archivo.read(&codigo[0], codigoSize);

        // Leemos el nombre del producto
        size_t nombreSize;
        archivo.read(reinterpret_cast<char*>(&nombreSize), sizeof(nombreSize));
        string nombre(nombreSize, '\0');
        archivo.read(&nombre[0], nombreSize);

        // Leemos precio, costo y existencias
        double precio, costo;
        int existencias, tienda;
        archivo.read(reinterpret_cast<char*>(&precio), sizeof(precio));
        archivo.read(reinterpret_cast<char*>(&costo), sizeof(costo));
        archivo.read(reinterpret_cast<char*>(&existencias), sizeof(existencias));
        archivo.read(reinterpret_cast<char*>(&tienda), sizeof(tienda));

        // Leemos el estatus
        size_t estatusSize;
        archivo.read(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
        string estatus(estatusSize, '\0');
        archivo.read(&estatus[0], estatusSize);

        // Leemos la fecha de cambio de estatus
        size_t fechaCambioEstatusSize;
        archivo.read(reinterpret_cast<char*>(&fechaCambioEstatusSize), sizeof(fechaCambioEstatusSize));
        string fechaCambioEstatus(fechaCambioEstatusSize, '\0');
        archivo.read(&fechaCambioEstatus[0], fechaCambioEstatusSize);

        // Creamos el nuevo producto
        try {
            nuevoProducto = new Producto(codigo, nombre, precio, costo, existencias, tienda);
            nuevoProducto->estatus = estatus;
            nuevoProducto->fechaCambioEstatus = fechaCambioEstatus;
        }
        catch (const std::invalid_argument&) {
            // Si el producto no es válido, continuamos con el siguiente
            continue;
        }

        // Añadimos el nuevo producto a la lista
        if (cabeza == nullptr) {
            cabeza = nuevoProducto;
            cola = nuevoProducto;
        }
        else {
            cola->siguiente = nuevoProducto;
            nuevoProducto->anterior = cola;
            cola = nuevoProducto;
        }
    }

    archivo.close();
    return cabeza;
}


void mostrarProductosEnListBox(Producto* cabeza, HWND hwndListBox) {
    if (cabeza == nullptr) {
        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)"No hay productos");
        MessageBox(nullptr, "No hay productos para mostrar.", "Información", MB_ICONINFORMATION);
        return;
    }

    // Limpiar el ListBox antes de agregar nuevos elementos
    SendMessage(hwndListBox, LB_RESETCONTENT, 0, 0);

    Producto* actual = cabeza;
    while (actual != nullptr) {
        // Formatear la cadena para mostrar la información del producto
        std::string productoInfo = (actual->codigo) + " " + actual->nombre;

        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)productoInfo.c_str());
        actual = actual->siguiente;
    }
}

void mostrarProductosEnListBoxUsuario(Producto* cabeza, HWND hwndListBox) {
    if (cabeza == nullptr) {
        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)"No hay productos");
        MessageBox(nullptr, "No hay productos para mostrar.", "Información", MB_ICONINFORMATION);
        return;
    }

    // Limpiar el ListBox antes de agregar nuevos elementos
    SendMessage(hwndListBox, LB_RESETCONTENT, 0, 0);

    Producto* actual = cabeza;
    while (actual != nullptr) {
        // Formatear la cadena para mostrar la información del producto
        std::string productoInfo = actual->nombre;

        SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)productoInfo.c_str());
        actual = actual->siguiente;
    }
}


bool esNumero(const char* cadena) {
    for (int i = 0; cadena[i] != '\0'; i++) {
        if (!isdigit(cadena[i]) && cadena[i] != '.') {
            return false; // Si el carácter no es un dígito o un punto decimal, no es un número
        }
    }
    return true;
}


//---------------------------------------------Gestion de COMPRAS ---------------------------------------------------------------

void agregarCompra(Compra*& cabeza, int id, const string& producto, const string& direccion,
    int cantidad, double precioUnitario, const string& fechaAprox, const string& cliente, const string& tienda,
    int idTienda, int idCliente, const string& idProducto) {
    if (cantidad <= 0) {
        MessageBox(nullptr, "No se puede agregar la compra. La cantidad debe ser mayor a 0.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    // Verificar si ya existe una compra con el mismo ID (si aplica)
    Compra* actual = cabeza;
    while (actual != nullptr) {
        if (actual->id == id) {
            MessageBox(nullptr, "No se puede agregar la compra. Ya existe una compra con el mismo ID.", "Error", MB_OK | MB_ICONERROR);
            return;
        }
        actual = actual->siguiente;
    }

    // Calcular el precio total
    double precioTotal = cantidad * precioUnitario;

    // Crear la nueva compra
    Compra* nuevaCompra = new Compra(id, producto, direccion, cantidad, precioUnitario, precioTotal, fechaAprox, cliente, tienda, idTienda, idCliente, idProducto);

    if (cabeza == nullptr) {
        // Si la lista está vacía, la nueva compra será la cabeza
        cabeza = nuevaCompra;
    }
    else {
        // Insertar al final de la lista
        actual = cabeza;
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevaCompra;
        nuevaCompra->anterior = actual;
    }
}


void liberarMemoriaCompras(Compra*& cabeza) {
    while (cabeza != nullptr) {
        Compra* temp = cabeza;
        cabeza = cabeza->siguiente;
        delete temp; // Liberar el nodo actual
    }
}

void guardarCompras(Compra* cabeza, const string& nombreArchivo) {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo de COMPRAS para guardar", "Error", MB_ICONERROR);
        return;
    }

    Compra* actual = cabeza;
    while (actual != nullptr) {
        // Escribir datos simples
        archivo.write(reinterpret_cast<char*>(&actual->id), sizeof(actual->id));
        archivo.write(reinterpret_cast<char*>(&actual->cantidad), sizeof(actual->cantidad));
        archivo.write(reinterpret_cast<char*>(&actual->precioUnitario), sizeof(actual->precioUnitario));
        archivo.write(reinterpret_cast<char*>(&actual->precioTotal), sizeof(actual->precioTotal));
        archivo.write(reinterpret_cast<char*>(&actual->idTienda), sizeof(actual->idTienda));
        archivo.write(reinterpret_cast<char*>(&actual->idCliente), sizeof(actual->idCliente));

        // Escribir strings con longitud
        size_t size;

        size = actual->producto.size();
        archivo.write(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.write(actual->producto.c_str(), size);

        size = actual->direccion.size();
        archivo.write(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.write(actual->direccion.c_str(), size);

        size = actual->fechaCompra.size();
        archivo.write(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.write(actual->fechaCompra.c_str(), size);

        size = actual->fechaAprox.size();
        archivo.write(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.write(actual->fechaAprox.c_str(), size);

        size = actual->estatus.size();
        archivo.write(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.write(actual->estatus.c_str(), size);

        size = actual->cliente.size();
        archivo.write(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.write(actual->cliente.c_str(), size);

        size = actual->tienda.size();
        archivo.write(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.write(actual->tienda.c_str(), size);

        size = actual->idProducto.size();
        archivo.write(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.write(actual->idProducto.c_str(), size);

        actual = actual->siguiente;
    }

    archivo.close();
}

Compra* cargarCompras(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo de COMPRAS para cargar", "Error", MB_ICONERROR);
        return nullptr;
    }

    Compra* cabeza = nullptr;
    Compra* cola = nullptr;

    while (true) {
        Compra* nuevaCompra = new Compra(0, "", "", 0, 0.0, 0.0, "", "", "", 0, 0, ""); // Nodo temporal

        // Leer datos simples
        archivo.read(reinterpret_cast<char*>(&nuevaCompra->id), sizeof(nuevaCompra->id));
        if (archivo.eof()) {
            delete nuevaCompra;
            break;
        }

        // Actualizar el último ID si el actual es mayor
        if (nuevaCompra->id > ultimoIdCompra) {
            ultimoIdCompra = nuevaCompra->id;
        }

        archivo.read(reinterpret_cast<char*>(&nuevaCompra->cantidad), sizeof(nuevaCompra->cantidad));
        archivo.read(reinterpret_cast<char*>(&nuevaCompra->precioUnitario), sizeof(nuevaCompra->precioUnitario));
        archivo.read(reinterpret_cast<char*>(&nuevaCompra->precioTotal), sizeof(nuevaCompra->precioTotal));
        archivo.read(reinterpret_cast<char*>(&nuevaCompra->idTienda), sizeof(nuevaCompra->idTienda));
        archivo.read(reinterpret_cast<char*>(&nuevaCompra->idCliente), sizeof(nuevaCompra->idCliente));

        // Leer strings con longitud
        size_t size;
        char buffer[256]; // Buffer temporal para strings

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevaCompra->producto = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevaCompra->direccion = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevaCompra->fechaCompra = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevaCompra->fechaAprox = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevaCompra->estatus = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevaCompra->cliente = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevaCompra->tienda = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevaCompra->idProducto = buffer;

        // Enlazar en la lista
        nuevaCompra->siguiente = nullptr;
        nuevaCompra->anterior = cola;
        if (cola != nullptr) {
            cola->siguiente = nuevaCompra;
        }
        else {
            cabeza = nuevaCompra;
        }
        cola = nuevaCompra;
    }

    archivo.close();
    return cabeza;
}


void cargarComprasEnListView(Compra* cabezaCompras, HWND hwndListView) {
    Compra* actual = cabezaCompras;
    int index = 0; // Contador para las filas del ListView

    // Recorremos la lista de compras
    while (actual != nullptr) {
        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = index; // Fila
        lvItem.iSubItem = 0;  // Primera columna (Producto)

        // Convertir el string producto a un arreglo de char
        char producto[200];  // Asegúrate de que el tamaño sea adecuado
        strcpy_s(producto, sizeof(producto), actual->producto.c_str());
        lvItem.pszText = producto;

        ListView_InsertItem(hwndListView, &lvItem);

        // Insertar los datos de cada columna
        char cantidad[20];
        char precioTotal[20];
        char fechaCompra[30];
        char estatus[30];
        char tienda[30];
        char cliente[30];

        // Convertir datos a cadenas de caracteres
        _itoa(actual->cantidad, cantidad, 10);    // Convertir cantidad a cadena
        sprintf_s(precioTotal, "%.2f", actual->precioTotal);  // Convertir precioTotal a cadena
        strcpy_s(fechaCompra, sizeof(fechaCompra), actual->fechaCompra.c_str());
        strcpy_s(estatus, sizeof(estatus), actual->estatus.c_str());
        strcpy_s(tienda, sizeof(tienda), actual->tienda.c_str());
        strcpy_s(cliente, sizeof(cliente), actual->cliente.c_str());

        // Insertar las demás columnas
        ListView_SetItemText(hwndListView, index, 1, cantidad);    // Cantidad
        ListView_SetItemText(hwndListView, index, 2, precioTotal);  // Precio Total
        ListView_SetItemText(hwndListView, index, 3, fechaCompra);  // Fecha Compra
        ListView_SetItemText(hwndListView, index, 4, estatus);     // Estatus
        ListView_SetItemText(hwndListView, index, 5, tienda);      // Tienda
        ListView_SetItemText(hwndListView, index, 6, cliente);     // Cliente

        // Avanzar al siguiente nodo
        actual = actual->siguiente;
        index++; // Aumentar el índice de la fila
    }
}

void cargarComprasEnListViewCliente(Compra* cabezaCompras, HWND hwndListView) {
    Compra* actual = cabezaCompras;
    int index = 0; // Contador para las filas del ListView

    // Recorremos la lista de compras
    while (actual != nullptr) {
        if (actual->idCliente == usuarioLogueado) {
            LVITEM lvItem = { 0 };
            lvItem.mask = LVIF_TEXT;
            lvItem.iItem = index; // Fila
            lvItem.iSubItem = 0;  // Primera columna (Producto)

            // Convertir el string producto a un arreglo de char
            char producto[200];  // Asegúrate de que el tamaño sea adecuado
            strcpy_s(producto, sizeof(producto), actual->producto.c_str());
            lvItem.pszText = producto;

            ListView_InsertItem(hwndListView, &lvItem);

            // Insertar los datos de cada columna
            char cantidad[20];
            char precioTotal[20];
            char fechaCompra[30];
            char estatus[30];
            char tienda[30];
            char cliente[30];

            // Convertir datos a cadenas de caracteres
            _itoa(actual->cantidad, cantidad, 10);    // Convertir cantidad a cadena
            sprintf_s(precioTotal, "%.2f", actual->precioTotal);  // Convertir precioTotal a cadena
            strcpy_s(fechaCompra, sizeof(fechaCompra), actual->fechaCompra.c_str());
            strcpy_s(estatus, sizeof(estatus), actual->estatus.c_str());
            strcpy_s(tienda, sizeof(tienda), actual->tienda.c_str());
            strcpy_s(cliente, sizeof(cliente), actual->cliente.c_str());

            // Insertar las demás columnas
            ListView_SetItemText(hwndListView, index, 1, cantidad);    // Cantidad
            ListView_SetItemText(hwndListView, index, 2, precioTotal);  // Precio Total
            ListView_SetItemText(hwndListView, index, 3, fechaCompra);  // Fecha Compra
            ListView_SetItemText(hwndListView, index, 4, estatus);     // Estatus
            ListView_SetItemText(hwndListView, index, 5, tienda);      // Tienda
            ListView_SetItemText(hwndListView, index, 6, cliente);     // Cliente

            index++; // Aumentar el índice de la fila
        }
        // Avanzar al siguiente nodo
        actual = actual->siguiente;
    }
}

void cargarComprasEnListViewTienda(Compra* cabezaCompras, HWND hwndListView, int tienda) {
    Compra* actual = cabezaCompras;
    int index = 0; // Contador para las filas del ListView

    // Recorremos la lista de compras
    while (actual != nullptr) {
        if (actual->idTienda == tienda) {
            LVITEM lvItem = { 0 };
            lvItem.mask = LVIF_TEXT;
            lvItem.iItem = index; // Fila
            lvItem.iSubItem = 0;  // Primera columna (Producto)

            // Convertir el string producto a un arreglo de char
            char producto[200];  // Asegúrate de que el tamaño sea adecuado
            strcpy_s(producto, sizeof(producto), actual->producto.c_str());
            lvItem.pszText = producto;

            ListView_InsertItem(hwndListView, &lvItem);

            // Insertar los datos de cada columna
            char cantidad[20];
            char precioTotal[20];
            char fechaCompra[30];
            char estatus[30];
            char tienda[30];
            char cliente[30];

            // Convertir datos a cadenas de caracteres
            _itoa(actual->cantidad, cantidad, 10);    // Convertir cantidad a cadena
            sprintf_s(precioTotal, "%.2f", actual->precioTotal);  // Convertir precioTotal a cadena
            strcpy_s(fechaCompra, sizeof(fechaCompra), actual->fechaCompra.c_str());
            strcpy_s(estatus, sizeof(estatus), actual->estatus.c_str());
            strcpy_s(tienda, sizeof(tienda), actual->tienda.c_str());
            strcpy_s(cliente, sizeof(cliente), actual->cliente.c_str());

            // Insertar las demás columnas
            ListView_SetItemText(hwndListView, index, 1, cantidad);    // Cantidad
            ListView_SetItemText(hwndListView, index, 2, precioTotal);  // Precio Total
            ListView_SetItemText(hwndListView, index, 3, fechaCompra);  // Fecha Compra
            ListView_SetItemText(hwndListView, index, 4, estatus);     // Estatus
            ListView_SetItemText(hwndListView, index, 5, tienda);      // Tienda
            ListView_SetItemText(hwndListView, index, 6, cliente);     // Cliente

            index++; // Aumentar el índice de la fila
        }
        // Avanzar al siguiente nodo
        actual = actual->siguiente;
    }
}

void cargarComprasEnListViewProducto(Compra* cabezaCompras, HWND hwndListView, string producto) {
    Compra* actual = cabezaCompras;
    int index = 0; // Contador para las filas del ListView

    // Recorremos la lista de compras
    while (actual != nullptr) {
        if (actual->idProducto == producto) {
            LVITEM lvItem = { 0 };
            lvItem.mask = LVIF_TEXT;
            lvItem.iItem = index; // Fila
            lvItem.iSubItem = 0;  // Primera columna (Producto)

            // Convertir el string producto a un arreglo de char
            char producto[200];  
            strcpy_s(producto, sizeof(producto), actual->producto.c_str());
            lvItem.pszText = producto;

            ListView_InsertItem(hwndListView, &lvItem);

            // Insertar los datos de cada columna
            char cantidad[20];
            char precioTotal[20];
            char fechaCompra[30];
            char estatus[30];
            char tienda[30];
            char cliente[30];

            // Convertir datos a cadenas de caracteres
            _itoa(actual->cantidad, cantidad, 10);    // Convertir cantidad a cadena
            sprintf_s(precioTotal, "%.2f", actual->precioTotal);  // Convertir precioTotal a cadena
            strcpy_s(fechaCompra, sizeof(fechaCompra), actual->fechaCompra.c_str());
            strcpy_s(estatus, sizeof(estatus), actual->estatus.c_str());
            strcpy_s(tienda, sizeof(tienda), actual->tienda.c_str());
            strcpy_s(cliente, sizeof(cliente), actual->cliente.c_str());

            // Insertar las demás columnas
            ListView_SetItemText(hwndListView, index, 1, cantidad);    // Cantidad
            ListView_SetItemText(hwndListView, index, 2, precioTotal);  // Precio Total
            ListView_SetItemText(hwndListView, index, 3, fechaCompra);  // Fecha Compra
            ListView_SetItemText(hwndListView, index, 4, estatus);     // Estatus
            ListView_SetItemText(hwndListView, index, 5, tienda);      // Tienda
            ListView_SetItemText(hwndListView, index, 6, cliente);     // Cliente

            index++; // Aumentar el índice de la fila
        }
        // Avanzar al siguiente nodo
        actual = actual->siguiente;
    }
}


//---------------------------------------------Gestion de COMPRAS ---------------------------------------------------------------

void agregarEnvio(Envios*& cabeza, int id, int idCompra, const string& producto, const string& direccion, const string& cliente) {
    // Validar que no exista un ID duplicado
    Envios* actual = cabeza;
    while (actual != nullptr) {
        if (actual->id == id) {
            MessageBox(nullptr, "El ID está repetido", "Error", MB_ICONERROR);
            return; // Salir si se encuentra un ID duplicado
        }
        actual = actual->siguiente;
    }

    // Crear un nuevo envío
    Envios* nuevoEnvio = new Envios(id, idCompra, producto, direccion, cliente);

    if (cabeza == nullptr) {
        // Si la lista está vacía, el nuevo envío será la cabeza
        cabeza = nuevoEnvio;
    }
    else {
        // Insertar al final de la lista
        actual = cabeza;
        while (actual->siguiente != nullptr) {
            actual = actual->siguiente;
        }
        actual->siguiente = nuevoEnvio;
        nuevoEnvio->anterior = actual;
    }
}

void guardarEnvios(Envios* cabeza, const string& nombreArchivo) {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo de envíos", "Error", MB_ICONERROR);
        return;
    }

    Envios* actual = cabeza;
    while (actual != nullptr) {
        archivo.write(reinterpret_cast<char*>(&actual->id), sizeof(actual->id));
        archivo.write(reinterpret_cast<char*>(&actual->idCompra), sizeof(actual->idCompra));

        size_t productoSize = actual->producto.size();
        archivo.write(reinterpret_cast<char*>(&productoSize), sizeof(productoSize));
        archivo.write(actual->producto.c_str(), productoSize);

        size_t direccionSize = actual->direccion.size();
        archivo.write(reinterpret_cast<char*>(&direccionSize), sizeof(direccionSize));
        archivo.write(actual->direccion.c_str(), direccionSize);

        size_t estatusSize = actual->estatus.size();
        archivo.write(reinterpret_cast<char*>(&estatusSize), sizeof(estatusSize));
        archivo.write(actual->estatus.c_str(), estatusSize);

        size_t fechaSize = actual->fechaActualizacion.size();
        archivo.write(reinterpret_cast<char*>(&fechaSize), sizeof(fechaSize));
        archivo.write(actual->fechaActualizacion.c_str(), fechaSize);

        size_t clienteSize = actual->cliente.size();
        archivo.write(reinterpret_cast<char*>(&clienteSize), sizeof(clienteSize));
        archivo.write(actual->cliente.c_str(), clienteSize);

        actual = actual->siguiente;
    }

    archivo.close();
}

Envios* cargarEnvios(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        MessageBox(nullptr, "No se pudo abrir el archivo de ENVÍOS para cargar", "Error", MB_ICONERROR);
        return nullptr;
    }

    Envios* cabeza = nullptr;
    Envios* cola = nullptr;

    while (true) {
        // Crear un nuevo nodo temporal
        Envios* nuevoEnvio = new Envios(0, 0, "", "", "");

        // Leer datos simples
        archivo.read(reinterpret_cast<char*>(&nuevoEnvio->id), sizeof(nuevoEnvio->id));
        if (archivo.eof()) {
            delete nuevoEnvio;
            break;
        }

        // Actualizar el último ID si el actual es mayor
        if (nuevoEnvio->id > ultimoIdEnvio) {
            ultimoIdEnvio = nuevoEnvio->id;
        }

        archivo.read(reinterpret_cast<char*>(&nuevoEnvio->idCompra), sizeof(nuevoEnvio->idCompra));

        // Leer strings con longitud
        size_t size;
        char buffer[256]; // Buffer temporal para strings

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevoEnvio->producto = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevoEnvio->direccion = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevoEnvio->estatus = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevoEnvio->fechaActualizacion = buffer;

        archivo.read(reinterpret_cast<char*>(&size), sizeof(size));
        archivo.read(buffer, size);
        buffer[size] = '\0';
        nuevoEnvio->cliente = buffer;

        // Enlazar en la lista
        nuevoEnvio->siguiente = nullptr;
        nuevoEnvio->anterior = cola;
        if (cola != nullptr) {
            cola->siguiente = nuevoEnvio;
        }
        else {
            cabeza = nuevoEnvio;
        }
        cola = nuevoEnvio;
    }

    archivo.close();
    return cabeza;
}

void liberarMemoriaEnvios(Envios*& cabeza) {
    while (cabeza != nullptr) {
        Envios* temp = cabeza;
        cabeza = cabeza->siguiente;
        delete temp;
    }
}

void cargarEnviosEnListView(Envios* cabezaEnvios, HWND hwndListView) {
    Envios* actual = cabezaEnvios;
    int index = 0; // Contador para las filas del ListView

    // Recorrer la lista de envíos
    while (actual != nullptr) {
        LVITEM lvItem = { 0 };
        lvItem.mask = LVIF_TEXT;
        lvItem.iItem = index; // Índice de la fila
        lvItem.iSubItem = 0;  // Primera columna (ID)

        // Convertir el ID a un arreglo de char
        char id[20];
        _itoa(actual->id, id, 10);
        lvItem.pszText = id;

        // Insertar la fila inicial con el ID
        ListView_InsertItem(hwndListView, &lvItem);

        // Preparar las columnas restantes
        char producto[200];
        char direccion[200];
        char estatus[50];
        char fechaActualizacion[50];
        char cliente[100];

        strcpy_s(producto, sizeof(producto), actual->producto.c_str());
        strcpy_s(direccion, sizeof(direccion), actual->direccion.c_str());
        strcpy_s(estatus, sizeof(estatus), actual->estatus.c_str());
        strcpy_s(fechaActualizacion, sizeof(fechaActualizacion), actual->fechaActualizacion.c_str());
        strcpy_s(cliente, sizeof(cliente), actual->cliente.c_str());

        // Insertar los datos en las columnas
        ListView_SetItemText(hwndListView, index, 1, producto);           // Producto
        ListView_SetItemText(hwndListView, index, 2, direccion);          // Dirección
        ListView_SetItemText(hwndListView, index, 3, estatus);           // Estatus
        ListView_SetItemText(hwndListView, index, 4, fechaActualizacion); // Fecha Actualización
        ListView_SetItemText(hwndListView, index, 5, cliente);           // Cliente

        // Avanzar al siguiente nodo
        actual = actual->siguiente;
        index++; // Incrementar el índice de la fila
    }
}

void cargarEnviosEnListViewClientes(Envios* cabezaEnvios, HWND hwndListView) {
    Envios* actual = cabezaEnvios;
    int index = 0; // Contador para las filas del ListView

    // Recorrer la lista de envíos
    while (actual != nullptr) {

        if (clienteComprar == actual->cliente) {
            LVITEM lvItem = { 0 };
            lvItem.mask = LVIF_TEXT;
            lvItem.iItem = index; // Índice de la fila
            lvItem.iSubItem = 0;  // Primera columna (ID)

            // Convertir el ID a un arreglo de char
            char id[20];
            _itoa(actual->id, id, 10);
            lvItem.pszText = id;

            // Insertar la fila inicial con el ID
            ListView_InsertItem(hwndListView, &lvItem);

            // Preparar las columnas restantes
            char producto[200];
            char direccion[200];
            char estatus[50];
            char fechaActualizacion[50];
            char cliente[100];

            strcpy_s(producto, sizeof(producto), actual->producto.c_str());
            strcpy_s(direccion, sizeof(direccion), actual->direccion.c_str());
            strcpy_s(estatus, sizeof(estatus), actual->estatus.c_str());
            strcpy_s(fechaActualizacion, sizeof(fechaActualizacion), actual->fechaActualizacion.c_str());
            strcpy_s(cliente, sizeof(cliente), actual->cliente.c_str());

            // Insertar los datos en las columnas
            ListView_SetItemText(hwndListView, index, 1, producto);           // Producto
            ListView_SetItemText(hwndListView, index, 2, direccion);          // Dirección
            ListView_SetItemText(hwndListView, index, 3, estatus);           // Estatus
            ListView_SetItemText(hwndListView, index, 4, fechaActualizacion); // Fecha Actualización
            ListView_SetItemText(hwndListView, index, 5, cliente);           // Cliente

            // Avanzar al siguiente nodo
            index++; // Incrementar el índice de la fila
        }
        actual = actual->siguiente;
        
    }
}


//------------------------------------------------------------METODOS DE ORDENAMIENTO----------------------------------------------------------
// 
// 
// 
//------------------------------------------------------------HEAPSORT----------------------------------------------------------

void heapify(Tienda** arr, int n, int i) {
    int largest = i;               // Inicialmente, la raíz es el más grande
    int left = 2 * i + 1;          // Hijo izquierdo
    int right = 2 * i + 2;         // Hijo derecho

    // Si el hijo izquierdo es mayor que la raíz
    if (left < n && arr[left]->id > arr[largest]->id) {
        largest = left;
    }

    // Si el hijo derecho es mayor que el más grande hasta ahora
    if (right < n && arr[right]->id > arr[largest]->id) {
        largest = right;
    }

    // Si el más grande no es la raíz
    if (largest != i) {
        std::swap(arr[i], arr[largest]); // Intercambiar
        heapify(arr, n, largest);       // Recursivamente aplicar a la subheap afectada
    }
}

void heapSort(Tienda** arr, int n) {
    // Construir el Max-Heap
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(arr, n, i);
    }

    // Extraer elementos del heap uno por uno
    for (int i = n - 1; i > 0; i--) {
        std::swap(arr[0], arr[i]); // Mover el mayor al final
        heapify(arr, i, 0);        // Restaurar el Max-Heap
    }
}

void ordenarTiendasPorID(Tienda*& cabeza) {
    if (cabeza == nullptr) {
        return; // Lista vacía, no hay nada que ordenar
    }

    // Paso 1: Convertir la lista ligada a un arreglo
    std::vector<Tienda*> tiendasArray;
    Tienda* actual = cabeza;
    while (actual != nullptr) {
        tiendasArray.push_back(actual);
        actual = actual->siguiente;
    }

    // Paso 2: Aplicar HeapSort al arreglo
    int n = tiendasArray.size();
    heapSort(tiendasArray.data(), n);

    // Paso 3: Reconstruir la lista ligada ordenada
    cabeza = tiendasArray[0];
    cabeza->anterior = nullptr;

    for (int i = 0; i < n - 1; i++) {
        tiendasArray[i]->siguiente = tiendasArray[i + 1];
        tiendasArray[i + 1]->anterior = tiendasArray[i];
    }

    tiendasArray[n - 1]->siguiente = nullptr; // Último nodo apunta a nullptr
}

//------------------------------------------------------------QUICKSORT----------------------------------------------------------

// Función para particionar la lista
Producto* partition(Producto* low, Producto* high) {
    double pivot = high->precio; // Elegimos el precio del último nodo como pivote
    Producto* i = low->anterior; // Nodo previo al rango

    for (Producto* j = low; j != high; j = j->siguiente) {
        if (j->precio <= pivot) {
            // Mover nodo más pequeño al lado izquierdo del pivote
            i = (i == nullptr) ? low : i->siguiente; // Avanzar si no es el primero
            std::swap(j->precio, i->precio);
            std::swap(j->codigo, i->codigo);
            std::swap(j->nombre, i->nombre);
            std::swap(j->costo, i->costo);
            std::swap(j->existencias, i->existencias);
            std::swap(j->estatus, i->estatus);
            std::swap(j->fechaCambioEstatus, i->fechaCambioEstatus);
            std::swap(j->tienda, i->tienda);
        }
    }

    // Colocar el pivote en la posición correcta
    i = (i == nullptr) ? low : i->siguiente;
    std::swap(i->precio, high->precio);
    std::swap(i->codigo, high->codigo);
    std::swap(i->nombre, high->nombre);
    std::swap(i->costo, high->costo);
    std::swap(i->existencias, high->existencias);
    std::swap(i->estatus, high->estatus);
    std::swap(i->fechaCambioEstatus, high->fechaCambioEstatus);
    std::swap(i->tienda, high->tienda);

    return i; // Retornar la nueva posición del pivote
}

// Función recursiva de QuickSort
void quickSort(Producto* low, Producto* high) {
    if (low != nullptr && high != nullptr && low != high && low != high->siguiente) {
        Producto* pivot = partition(low, high);
        quickSort(low, pivot->anterior);   // Ordenar la parte izquierda
        quickSort(pivot->siguiente, high); // Ordenar la parte derecha
    }
}

// Función para encontrar el último nodo en la lista
Producto* findLastNode(Producto* head) {
    Producto* temp = head;
    while (temp->siguiente != nullptr) {
        temp = temp->siguiente;
    }
    return temp;
}

// Función para ordenar la lista usando QuickSort
void sortProductos(Producto* cabezaProductos) {
    if (cabezaProductos == nullptr) return;

    Producto* lastNode = findLastNode(cabezaProductos); // Encontrar el último nodo
    quickSort(cabezaProductos, lastNode);              // Llamar a QuickSort
}


//--------------------------------------------------------------BUSQUEDA BINARIA------------------------------------------------------

Cliente* busquedaBinariaCliente(Cliente* raiz, int id) {
    // Si el árbol está vacío o hemos encontrado el cliente
    if (raiz == nullptr || raiz->id == id) {
        return raiz;  // Retorna el cliente encontrado o nullptr si no se encontró
    }

    // Si el ID a buscar es menor que el ID del nodo actual, busca en el subárbol izquierdo
    if (id < raiz->id) {
        return busquedaBinariaCliente(raiz->izquierda, id);
    }

    // Si el ID a buscar es mayor que el ID del nodo actual, busca en el subárbol derecho
    return busquedaBinariaCliente(raiz->derecha, id);
}
