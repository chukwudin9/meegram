#pragma once

#include <td/telegram/td_api.h>

#include <QAbstractItemModel>
#include <QList>

class ChatStore;

class ChatModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ChatModel(ChatStore *store, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    ChatStore *m_store{};
    QList<td::td_api::chat *> m_data;
};
