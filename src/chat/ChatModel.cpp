#include "ChatModel.h"

ChatModel::ChatModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

int ChatModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return {};
}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    return QVariant();
}
