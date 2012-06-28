#include "XProfileWidget"
#include "QTreeWidget"
#include "QListWidget"
#include "QVBoxLayout"
#include "QHeaderView"
#include "QPushButton"
#include "XProfiler"
#include "QCheckBox"
#include "QStackedLayout"
#include "QMenu"
#include "QDebug"

class Item : public QTreeWidgetItem
  {
XProperties:
  XROProperty(const char *, message)
  XRORefProperty(XTimeStatistics, statistics)
public:
  Item(const char *msg, const XTimeStatistics &stats) : _message(msg), _statistics(stats)
    {
    xAssert(msg);
    }

  virtual QVariant data( int column, int role ) const
    {
    if(role == Qt::DisplayRole)
      {
      if(column == 0) return message();
      if(column == 1) return (quint64) statistics().count();
      if(column == 2) return statistics().total().milliseconds();
      if(column == 3) return statistics().minimum().milliseconds();
      if(column == 4) return statistics().average().milliseconds();
      if(column == 5) return statistics().maximum().milliseconds();
      }
    return QTreeWidgetItem::data(column, role);
    }
  };

XProfilerWidget::XProfilerWidget(QWidget *parent) : QWidget(parent)
  {
  QVBoxLayout *layout(new QVBoxLayout(this));
  layout->setContentsMargins(2, 2, 2, 2);

  _stackedLayout = new QStackedLayout();
  layout->addLayout(_stackedLayout);


  // tree
  _tree = new QTreeWidget(this);
  _stackedLayout->addWidget(_tree);
  _tree->setHeaderLabels(QStringList() << "Item" << "Count" << "Total (ms)" << "Minimum (ms)" << "Average (ms)" << "Maximum (ms)");


  // list
  _list = new QTreeWidget(this);
  _stackedLayout->addWidget(_list);
  _list->setHeaderLabels(QStringList() << "Item" << "Count" << "Total (ms)" << "Minimum (ms)" << "Average (ms)" << "Maximum (ms)");



  // shared buttons
  QHBoxLayout *buttonLayout(new QHBoxLayout());
  buttonLayout->setContentsMargins(0, 0, 0, 0);
  layout->addLayout(buttonLayout);
  buttonLayout->addStretch();

  _showTree = new QCheckBox("Show Call Tree");
  _showTree->setChecked(true);
  buttonLayout->addWidget(_showTree);
  connect(_showTree, SIGNAL(clicked()), this, SLOT(update()));

  QPushButton *filterButton(new QPushButton("Edit Filter"));
  buttonLayout->addWidget(filterButton);
  connect(filterButton, SIGNAL(clicked()), this, SLOT(chooseFilter()));
  QPushButton *clearButton(new QPushButton("Clear"));
  buttonLayout->addWidget(clearButton);
  connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));
  QPushButton *updateButton(new QPushButton("Update"));
  buttonLayout->addWidget(updateButton);
  connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));

  update();
  }

void XProfilerWidget::clear()
  {
  XProfiler::clearResults();
  update();
  }

void XProfilerWidget::chooseFilter()
  {
  QList <xuint32> types;
  findAllTypes(XProfiler::rootContext(), types);

  QMenu menu;

  foreach(xuint32 t, types)
    {
    if(t != X_UINT32_SENTINEL)
      {
      QAction* act = menu.addAction(XProfiler::stringForContext(t));
      act->setCheckable(true);
      act->setData(t);
      act->setChecked(!_filteredTypes.contains(t));
      }
    }
  connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(setFilter(QAction*)));

  menu.exec(QCursor::pos());
  }

void XProfilerWidget::setFilter(QAction *enabled)
  {
  xuint32 t = enabled->data().toUInt();
  if(!enabled->isChecked() && !_filteredTypes.contains(t))
    {
    _filteredTypes << t;
    }
  else
    {
    _filteredTypes.removeAll(t);
    }

  update();
  }

void XProfilerWidget::update()
  {
  _tree->clear();

  if(_showTree->isChecked())
    {
    _tree->setSortingEnabled(false);
    populateTreeFromContext(XProfiler::rootContext());
    _stackedLayout->setCurrentWidget(_tree);
    _tree->setSortingEnabled(true);
    }
  else
    {
    _list->setSortingEnabled(false);
    populateListFromContext(XProfiler::rootContext());
    _stackedLayout->setCurrentWidget(_list);
    _list->setSortingEnabled(true);
    }
  }

void XProfilerWidget::populateTreeFromContext(const XProfiler::ProfilingContext* ctx, QTreeWidgetItem* parent)
  {
  if(ctx && !_filteredTypes.contains(ctx->context()))
    {
    QTreeWidgetItem* item = parent;
    if(strcmp(ctx->message(), "") != 0)
      {
      item = new Item(ctx->message(), ctx->timeStats());

      if(parent)
        {
        parent->addChild(item);
        }
      else
        {
        _tree->addTopLevelItem(item);
        }
      }

    XProfiler::ProfilingContext* child=ctx->firstChild();
    while(child)
      {
      populateTreeFromContext(child, item);
      child = child->nextSibling();
      }
    }
  }

void XProfilerWidget::populateListFromContext(const XProfiler::ProfilingContext* ctx)
  {
  QHash<const char *, XTimeStatistics> items;
  populateHashFromContext(ctx, items);

  _list->clear();
  foreach(const char *fn, items.keys())
    {
    const XTimeStatistics& stats = items.value(fn);

    if(fn != 0 && stats.count() != 0)
      {
      _list->addTopLevelItem(new Item(fn, stats));
      }
    }
  }

void XProfilerWidget::populateHashFromContext(const XProfiler::ProfilingContext* ctx, QHash<const char *, XTimeStatistics> &hash) const
  {
  if(ctx && !_filteredTypes.contains(ctx->context()))
    {
    hash[ctx->message()].append(ctx->timeStats());

    XProfiler::ProfilingContext* child=ctx->firstChild();
    while(child)
      {
      populateHashFromContext(child, hash);
      child = child->nextSibling();
      }
    }
  }

void XProfilerWidget::findAllTypes(const XProfiler::ProfilingContext *ctx, QList <xuint32> &types) const
  {
  if(!types.contains(ctx->context()))
    {
    types << ctx->context();
    }

  XProfiler::ProfilingContext* child=ctx->firstChild();
  while(child)
    {
    findAllTypes(child, types);
    child = child->nextSibling();
    }
  }
