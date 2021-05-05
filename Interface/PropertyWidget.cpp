#include "PropertyWidget.h"

#include "CollapsibleWidget.h"

#include "../Data/Property.h"

#include <QBoxLayout>
#include <QCheckBox>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>

#include <iostream>

namespace iseg {

static const int CHILD_INDENT = 8;
static const int ROW_HEIGHT = 20;

PropertyWidget::PropertyWidget(Property_ptr p, QWidget* parent, const char* name, Qt::WindowFlags wFlags)
		: QWidget(parent, name, wFlags)
{
	setProperty(p);
}

void PropertyWidget::setProperty(Property_ptr p)
{
	if (p && p != _property)
	{
		_property = p;

		_widgetPropertyMap.clear();
		_collapseButtonLayoutMap.clear();

		auto layout = new QVBoxLayout(this);
		build(_property, layout);
		layout->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
		setLayout(layout);
	}
}

QWidget* PropertyWidget::makePropertyUI(Property& prop)
{
	const auto make_line_edit = [this](const Property& prop) {
		// generic attributes
		auto edit = new QLineEdit;
		edit->setToolTip(QString::fromStdString(prop.ToolTip()));
		edit->setEnabled(prop.Enabled());
		edit->setVisible(prop.Visible());
		QObject::connect(edit, SIGNAL(editingFinished()), this, SLOT(edited()));
		return edit;
	};

	switch (prop.Type())
	{
	case Property::kInteger: {
		auto p = dynamic_cast<PropertyInt*>(&prop);
		auto edit = make_line_edit(prop);
		edit->setText(QString::number(p->Value())); // ? should this be set via Property::StringValue?
		edit->setValidator(new QIntValidator(p->MinValue(), p->MaxValue()));
		return edit;
	}
	case Property::kReal: {
		auto p = dynamic_cast<PropertyReal*>(&prop);
		auto edit = make_line_edit(prop);
		edit->setText(QString::number(p->Value())); // ? should this be set via Property::StringValue?
		edit->setValidator(new QDoubleValidator(p->MinValue(), p->MaxValue(), 10));
		return edit;
	}
	case Property::kString: {
		auto p = dynamic_cast<PropertyString*>(&prop);
		auto edit = make_line_edit(prop);
		edit->setText(QString::fromStdString(p->Value()));
		return edit;
	}
	case Property::kBool: {
		auto p = dynamic_cast<PropertyBool*>(&prop);
		auto checkbox = new QCheckBox;
		checkbox->setChecked(p->Value());
		checkbox->setStyleSheet("QCheckBox::indicator {width: 13px; height: 13px; }");
		// connect to signal
		QObject::connect(checkbox, SIGNAL(toggled(bool)), this, SLOT(edited()));
		return checkbox;
	}
	case Property::kButton: {
		auto p = dynamic_cast<PropertyButton*>(&prop);
		auto button = new QPushButton(QString::fromStdString(p->ButtonText()));
		button->setAutoDefault(false);
		// connect to signal
		QObject::connect(button, SIGNAL(released()), this, SLOT(edited()));
		return button;
	}
	default:
		return new QWidget; // \todo so we have two columns
	}
}

void PropertyWidget::build(Property_ptr prop, QBoxLayout* layout)
{
	const auto label_text = prop->Description().empty() ? prop->Name() : prop->Description();
	auto field = makePropertyUI(*prop);
	field->setMaximumHeight(ROW_HEIGHT);

	// for callbacks
	_widgetPropertyMap[field] = prop;

	if (prop->Type() == Property::kGroup)
	{
		auto collapse_button = new QToolButton;
		collapse_button->setStyleSheet("QToolButton { border: none; }");
		collapse_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
		collapse_button->setArrowType(Qt::ArrowType::DownArrow);
		//collapse_button->setText(QString::fromStdString(label_text));
		collapse_button->setCheckable(true);
		collapse_button->setChecked(true);
		collapse_button->setIconSize(QSize(5, 5));
		collapse_button->setMaximumHeight(ROW_HEIGHT);

		auto label = new QLabel(QString::fromStdString(label_text));
		label->setMaximumHeight(ROW_HEIGHT);

		auto header_hbox = new QHBoxLayout;
		header_hbox->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
		header_hbox->addWidget(collapse_button);
		header_hbox->addWidget(label);
		header_hbox->setSpacing(0);
		//header_hbox->setSizeConstraint(QLayout::SizeConstraint::)

		auto child_vbox = new QVBoxLayout;
		if (!prop->Properties().empty())
		{
			auto header_line = new QFrame;
			header_line->setFrameShape(QFrame::HLine);
			header_line->setFrameShadow(QFrame::Plain);
			//header_line->setStyleSheet("QFrame[frameShape=\"4\"] { color: rgb(128,128,128); }");
			header_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
			//child_vbox->addWidget(header_line);

			for (auto p : prop->Properties())
			{
				build(p, child_vbox);
			}
		}
		auto child_area = new QWidget;
		child_area->setLayout(child_vbox);

		auto collapse_vbox = new QVBoxLayout;
		collapse_vbox->addLayout(header_hbox);
		collapse_vbox->addWidget(child_area);
		collapse_vbox->setContentsMargins(CHILD_INDENT, 0, 0, 0);
		collapse_vbox->setSpacing(0);

		_collapseButtonLayoutMap[collapse_button] = collapse_vbox;

		layout->addLayout(collapse_vbox);

		QObject::connect(collapse_button, SIGNAL(clicked(bool)), this, SLOT(toggleCollapsable(bool)));
	}
	else
	{
		auto label = new QLabel(QString::fromStdString(label_text));

		// split at 50%
		QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
		sp.setHorizontalStretch(1);
		label->setSizePolicy(sp);
		field->setSizePolicy(sp);

		auto hbox = new QHBoxLayout;
		hbox->addWidget(label);
		hbox->addWidget(field);
		hbox->setSpacing(2);
		hbox->setContentsMargins(CHILD_INDENT, 0, 0, 0);

		layout->addLayout(hbox);
	}
}

void PropertyWidget::toggleCollapsable(bool checked)
{
	if (auto btn = dynamic_cast<QToolButton*>(QObject::sender()))
	{
		btn->setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);

		if (auto layout = _collapseButtonLayoutMap[btn])
		{
			auto last_item = layout->count() - 1;
			if (auto w = layout->itemAt(last_item)->widget())
			{
				auto animation = new QPropertyAnimation(w, "maximumHeight");
				animation->setDuration(100);
				animation->setStartValue(w->maximumHeight());
				animation->setEndValue(checked ? w->sizeHint().height() : 0);
				animation->start();

				QObject::connect(animation, SIGNAL(finished()), this, SLOT(update()));
			}
		}
	}
}

namespace {
template<typename T>
struct type
{
};

double toType(QString const& q, type<double>)
{
	return q.toDouble();
}

int toType(QString const& q, type<int>)
{
	return q.toInt();
}

std::string toType(QString const& q, type<std::string>)
{
	return q.toStdString();
}

template<typename T>
auto toType(QString const& q)
		-> decltype(toType(q, type<T>{}))
{
	return toType(q, type<T>{});
}
} // namespace

void PropertyWidget::edited()
{
	if (auto w = dynamic_cast<QWidget*>(QObject::sender()))
	{
		if (auto prop = std::dynamic_pointer_cast<Property>(_widgetPropertyMap[w].lock()))
		{
			switch (prop->Type())
			{
			case Property::kInteger: {
				if (auto edit = dynamic_cast<QLineEdit*>(w))
				{
					auto prop_typed = std::dynamic_pointer_cast<PropertyInt>(prop);
					auto v = toType<int>(edit->text());
					if (v != prop_typed->Value())
					{
						prop_typed->SetValue(v);
						onPropertyEdited(prop);
					}
				}
				break;
			}
			case Property::kReal: {
				if (auto edit = dynamic_cast<QLineEdit*>(w))
				{
					auto prop_typed = std::dynamic_pointer_cast<PropertyReal>(prop);
					auto v = toType<double>(edit->text());
					if (v != prop_typed->Value())
					{
						prop_typed->SetValue(v);
						onPropertyEdited(prop);
					}
				}
				break;
			}
			case Property::kString: {
				if (auto edit = dynamic_cast<QLineEdit*>(w))
				{
					auto prop_typed = std::dynamic_pointer_cast<PropertyString>(prop);
					auto v = toType<std::string>(edit->text());
					if (v != prop_typed->Value())
					{
						prop_typed->SetValue(v);
						onPropertyEdited(prop);
					}
				}
				break;
			}
			case Property::kBool: {
				if (auto cb = dynamic_cast<QCheckBox*>(w))
				{
					auto prop_typed = std::dynamic_pointer_cast<PropertyBool>(prop);
					auto v = cb->isChecked();
					if (v != prop_typed->Value())
					{
						prop_typed->SetValue(v);
						onPropertyEdited(prop);
					}
				}
				break;
			}
			case Property::kButton: {
				auto prop_typed = std::dynamic_pointer_cast<PropertyButton>(prop);
				if (prop_typed->Value())
				{
					prop_typed->Value()();
				}
				break;
			}
			default:
				break;
			}
		}
	}
}

} // namespace iseg