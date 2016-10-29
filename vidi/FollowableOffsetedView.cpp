#include "FollowableOffsetedView.h"

void FollowableOffsetedView::initContextMenu()
{

    m_contextActions[ACTION_FOLLOW] = new QAction("Follow", &m_ContextMenu);
    connect(m_contextActions[ACTION_FOLLOW], SIGNAL(triggered()), this, SLOT(followSelectedOffset()) );
    m_ContextMenu.addAction(m_contextActions[ACTION_FOLLOW]);
}

