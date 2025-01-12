START TRANSACTION;

CREATE TABLE IF NOT EXISTS Categories (
    CategoryID INT AUTO_INCREMENT,
    CategoryName VARCHAR(255),
    Description TEXT,
    PRIMARY KEY (CategoryID)
);

CREATE TABLE IF NOT EXISTS Customers (
    CustomerID INT AUTO_INCREMENT,
    CustomerName VARCHAR(255),
    ContactName VARCHAR(255),
    Address VARCHAR(255),
    City VARCHAR(255),
    PostalCode VARCHAR(20),
    Country VARCHAR(100),
    PRIMARY KEY (CustomerID)
);

CREATE TABLE IF NOT EXISTS Employees (
    EmployeeID INT AUTO_INCREMENT,
    LastName VARCHAR(255),
    FirstName VARCHAR(255),
    BirthDate DATE,
    Photo TEXT,
    Notes TEXT,
    PRIMARY KEY (EmployeeID)
);

CREATE TABLE IF NOT EXISTS Shippers (
    ShipperID INT AUTO_INCREMENT,
    ShipperName VARCHAR(255),
    Phone VARCHAR(20),
    PRIMARY KEY (ShipperID)
);

CREATE TABLE IF NOT EXISTS Suppliers (
    SupplierID INT AUTO_INCREMENT,
    SupplierName VARCHAR(255),
    ContactName VARCHAR(255),
    Address VARCHAR(255),
    City VARCHAR(255),
    PostalCode VARCHAR(20),
    Country VARCHAR(100),
    Phone VARCHAR(20),
    PRIMARY KEY (SupplierID)
);

CREATE TABLE IF NOT EXISTS Products (
    ProductID INT AUTO_INCREMENT,
    ProductName VARCHAR(255),
    SupplierID INT,
    CategoryID INT,
    Unit VARCHAR(255),
    Price DECIMAL(10,2) DEFAULT 0,
    FOREIGN KEY (CategoryID) REFERENCES Categories(CategoryID),
    FOREIGN KEY (SupplierID) REFERENCES Suppliers(SupplierID),
    PRIMARY KEY (ProductID)
);

CREATE TABLE IF NOT EXISTS Orders (
    OrderID INT AUTO_INCREMENT,
    CustomerID INT,
    EmployeeID INT,
    OrderDate DATETIME,
    ShipperID INT,
    FOREIGN KEY (EmployeeID) REFERENCES Employees(EmployeeID),
    FOREIGN KEY (CustomerID) REFERENCES Customers(CustomerID),
    FOREIGN KEY (ShipperID) REFERENCES Shippers(ShipperID),
    PRIMARY KEY (OrderID)
);

CREATE TABLE IF NOT EXISTS OrderDetails (
    OrderDetailID INT AUTO_INCREMENT,
    OrderID INT,
    ProductID INT,
    Quantity INT,
    FOREIGN KEY (OrderID) REFERENCES Orders(OrderID),
    FOREIGN KEY (ProductID) REFERENCES Products(ProductID),
    PRIMARY KEY (OrderDetailID)
);

COMMIT;
