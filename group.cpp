#include "group.h"
#include <QApplication>
#include <QCursor>
#include <QEvent>
#include <QGridLayout>
#include <QMouseEvent>
#include <QPushButton>
#include "ui_group.h"

Group::Group(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Group)
{
    ui->setupUi(this);

    //this->setFocusPolicy(Qt::StrongFocus);  // Make the window focusable
    //setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint); // Optionally keep window always on top, if needed
    //this->setWindowState(Qt::WindowActive);
    // Set the window to stay on top of other windows
    setWindowFlags(windowFlags() | Qt::Popup);

    // Install event filter to track mouse events globally
    QApplication::instance()->installEventFilter(this);

    // Get the current position of the mouse
    QPoint globalMousePos = QCursor::pos();

    // Calculate the position to center the window's top at the mouse position
    int windowWidth = this->width();
    int windowHeight = this->height();

    // Define the margin (top space) you want between the cursor and the window
    int marginTop = 40;

    // Move the window so that its top-center is at the mouse position, plus the margin
    int xPos = globalMousePos.x() - windowWidth / 2;
    int yPos = globalMousePos.y() - windowHeight - marginTop; // Add margin to the vertical position

    move(xPos, yPos);

    // Set the window to stay on top of other windows
    //setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    // Create a central widget (if not already done in the .ui file)
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create a grid layout
    QGridLayout *layout = new QGridLayout;

    // Create and add buttons to the layout
    int buttonCount = 2; // Example: Creating 2 buttons
    int row = 0, col = 0;

    for (int i = 0; i < buttonCount; ++i) {
        QPushButton *button = new QPushButton(QString("Button %1").arg(i + 1), this);

        // Set fixed size for the button (40x40)
        button->setFixedSize(40, 40);

        layout->addWidget(button, row, col); // Add button to the layout at (row, col)

        // Update column index and row index
        col++;
        if (col == 4) { // If we've added 4 buttons in a row, move to the next row
            col = 0;
            row++;
        }
    }

    // Set the layout to the central widget
    centralWidget->setLayout(layout);

    // Adjust the minimum window size to fit the layout and buttons
    int buttonWidth = 40;        // Fixed button width
    int buttonHeight = 40;       // Fixed button height
    int width = buttonWidth * 4; // Max 4 buttons in one row
    int height = (buttonCount / 4 + (buttonCount % 4 == 0 ? 0 : 1))
                 * buttonHeight; // Calculate required height
    setMinimumSize(width, height);

    // Install event filter for global mouse events
    QApplication::instance()->installEventFilter(this);
}

Group::~Group()
{
    delete ui;
}

void Group::focusOutEvent(QFocusEvent *event)
{
    QApplication::quit();              // Close the application when the window loses focus
    QMainWindow::focusOutEvent(event); // Call base class implementation if needed
}
