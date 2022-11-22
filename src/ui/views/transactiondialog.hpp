#pragma once

#include <string>
#include <adwaita.h>
#include "../../controllers/transactiondialogcontroller.hpp"

namespace NickvisionMoney::UI::Views
{
	/**
     * A dialog for managing a transaction
     */
 	class TransactionDialog
 	{
 	public:
 		/**
		 * Constructs a TransactionDialog
		 *
		 * @param parent The parent window for the dialog
		 * @param controller The TransactionDialogController
		 */
		TransactionDialog(GtkWindow* parent, NickvisionMoney::Controllers::TransactionDialogController& controller);
		/**
    	 * Gets the GtkWidget* representing the TransactionDialog
    	 *
    	 * @returns The GtkWidget* representing the TransactionDialog
    	 */
		GtkWidget* gobj();
		/**
    	 * Run the TransactionDialog
    	 *
    	 * @returns True if dialog was accepted, else false
    	 */
		bool run();

 	private:
 		NickvisionMoney::Controllers::TransactionDialogController& m_controller;
		GtkWidget* m_gobj;
		GtkWidget* m_boxMain{ nullptr };
		GtkWidget* m_preferencesGroupMain{ nullptr };
		GtkWidget* m_rowDescription{ nullptr };
		GtkWidget* m_rowAmount{ nullptr };
		GtkWidget* m_rowType{ nullptr };
		GtkWidget* m_boxType{ nullptr };
		GtkWidget* m_btnIncome{ nullptr };
		GtkWidget* m_btnExpense{ nullptr };
		GtkWidget* m_preferencesGroupDateRepeat{ nullptr };
		GtkWidget* m_rowDate{ nullptr };
		GtkWidget* m_btnDate{ nullptr };
		GtkWidget* m_popoverDate{ nullptr };
		GtkWidget* m_calendarDate{ nullptr };
		GtkWidget* m_rowRepeatInterval{ nullptr };
		GtkWidget* m_preferencesGroupGroupColor{ nullptr };
		GtkWidget* m_rowGroup{ nullptr };
		GtkWidget* m_rowColor{ nullptr };
		GtkWidget* m_btnColor{ nullptr };
		GtkWidget* m_preferencesGroupTransfer{ nullptr };
		GtkWidget* m_lblTransferNoRecent{ nullptr };
		GtkWidget* m_btnContentTransferOpenFile{ nullptr };
		GtkWidget* m_btnTransferOpenFile{ nullptr };
		GtkWidget* m_boxTransferOpen{ nullptr };
		GtkWidget* m_popoverTransferOpen{ nullptr };
		GtkWidget* m_popoverTransferHelp{ nullptr };
		GtkWidget* m_clampTransferHelp{ nullptr };
		GtkWidget* m_lblTransferHelp{ nullptr };
		GtkWidget* m_boxTransfer{ nullptr };
		GtkWidget* m_lblTransferTarget{ nullptr };
		GtkWidget* m_btnTransferHelp{ nullptr };
		GtkWidget* m_btnTransferOpen{ nullptr };
		GtkWidget* m_rowTransfer{ nullptr };
		GtkEventController* m_eventAmountKey{ nullptr };
		/**
    	 * Sets the response
    	 *
    	 * @param The new response
    	 */
		void setResponse(const std::string& response);
		/**
		 * Occurs when the date selected is changed
		 */
		void onDateChanged();
		/**
		 * Occurs when transaction type is changed
		 */
		void onTypeChanged();
		/**
		 * Occurs when a key is released in the amount entry row
		 *
		 * @param keyval The released key
		 * @param state The bitmask, representing the state of modifier keys and pointer buttons
		 */
		void onAmountKeyReleased(unsigned int keyval, GdkModifierType state);
 	};
}