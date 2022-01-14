#include "mainwindow.h"
#include <filesystem>
#include "../models/configuration.h"
#include "../controls/progressdialog.h"
#include "shortcutswindow.h"
#include "settingsdialog.h"
#include "transactiondialog.h"

namespace NickvisionMoney::Views
{
    using namespace NickvisionMoney::Models;
    using namespace NickvisionMoney::Controls;

    MainWindow::MainWindow() : m_opened(false), m_updater("https://raw.githubusercontent.com/nlogozzo/NickvisionMoney/main/UpdateConfig.json", { "2022.1.3" }), m_account(std::nullopt)
    {
        //==Settings==//
        set_default_size(800, 600);
        set_titlebar(m_headerBar);
        m_headerBar.setTitle("Nickvision Money");
        m_headerBar.setSubtitle("No Account Open");
        signal_show().connect(sigc::mem_fun(*this, &MainWindow::onShow));
        //==HeaderBar==//
        m_headerBar.getActionNewAccount()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::newAccount));
        m_headerBar.getActionOpenAccount()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::openAccount));
        m_headerBar.getActionCloseAccount()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::closeAccount));
        m_headerBar.getBtnNewTransaction().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::newTransaction));
        m_headerBar.getBtnEditTransaction().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::editTransaction));
        m_headerBar.getBtnDTDelete().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::deleteTransaction));
        m_headerBar.getBtnBackupAccount().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::backupAccount));
        m_headerBar.getBtnRestoreAccount().signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::restoreAccount));
        m_headerBar.getActionCheckForUpdates()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::checkForUpdates));
        m_headerBar.getActionGitHubRepo()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::gitHubRepo));
        m_headerBar.getActionReportABug()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::reportABug));
        m_headerBar.getActionSettings()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::settings));
        m_headerBar.getActionShortcuts()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::shortcuts));
        m_headerBar.getActionChangelog()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::changelog));
        m_headerBar.getActionAbout()->signal_activate().connect(sigc::mem_fun(*this, &MainWindow::about));
        m_headerBar.getActionCloseAccount()->set_enabled(false);
        m_headerBar.getBtnNewTransaction().set_sensitive(false);
        m_headerBar.getBtnEditTransaction().set_sensitive(false);
        m_headerBar.getBtnDeleteTransaction().set_sensitive(false);
        m_headerBar.getBtnBackupAccount().set_sensitive(false);
        m_headerBar.getBtnRestoreAccount().set_sensitive(false);
        //==Grid==//
        m_gridAccountInfo.set_margin(6);
        m_gridAccountInfo.insert_column(0);
        m_gridAccountInfo.insert_column(1);
        m_gridAccountInfo.insert_column(2);
        m_gridAccountInfo.insert_row(0);
        m_gridAccountInfo.insert_row(1);
        m_gridAccountInfo.set_column_spacing(6);
        m_gridAccountInfo.set_row_spacing(6);
        //Income
        m_lblIncome.set_text("Income");
        m_lblIncome.set_halign(Gtk::Align::START);
        m_txtIncome.set_placeholder_text("---");
        m_txtIncome.set_editable(false);
        m_txtIncome.set_size_request(310, -1);
        m_gridAccountInfo.attach(m_lblIncome, 0, 0);
        m_gridAccountInfo.attach(m_txtIncome, 0, 1);
        //Expense
        m_lblExpense.set_text("Expense");
        m_lblExpense.set_halign(Gtk::Align::START);
        m_txtExpense.set_placeholder_text("---");
        m_txtExpense.set_editable(false);
        m_txtExpense.set_size_request(310, -1);
        m_gridAccountInfo.attach(m_lblExpense, 1, 0);
        m_gridAccountInfo.attach(m_txtExpense, 1, 1);
        //Total
        m_lblTotal.set_text("Total");
        m_lblTotal.set_halign(Gtk::Align::START);
        m_txtTotal.set_placeholder_text("---");
        m_txtTotal.set_editable(false);
        m_txtTotal.set_size_request(310, -1);
        m_gridAccountInfo.attach(m_lblTotal, 2, 0);
        m_gridAccountInfo.attach(m_txtTotal, 2, 1);
        //==Data Transactions==//
        m_dataTransactionsModel = Gtk::ListStore::create(m_dataTransactionsColumns);
        m_dataTransactions.append_column("ID", m_dataTransactionsColumns.getColID());
        m_dataTransactions.append_column("Date", m_dataTransactionsColumns.getColDate());
        m_dataTransactions.append_column("Description", m_dataTransactionsColumns.getColDescription());
        m_dataTransactions.append_column("Type", m_dataTransactionsColumns.getColType());
        m_dataTransactions.append_column("Repeat Interval", m_dataTransactionsColumns.getColRepeatInterval());
        m_dataTransactions.append_column("Amount", m_dataTransactionsColumns.getColAmount());
        m_dataTransactions.set_model(m_dataTransactionsModel);
        m_dataTransactions.get_selection()->set_mode(Gtk::SelectionMode::SINGLE);
        m_dataTransactions.set_activate_on_single_click(false);
        m_dataTransactions.signal_row_activated().connect(sigc::mem_fun(*this, &MainWindow::onRowDoubleClick));
        m_dataTransactions.get_selection()->signal_changed().connect([&]()
        {
           if(m_dataTransactions.get_selection()->get_selected_rows().size() == 1)
           {
               m_headerBar.getBtnEditTransaction().set_sensitive(true);
               m_headerBar.getBtnDeleteTransaction().set_sensitive(true);
           }
           else
           {
               m_headerBar.getBtnEditTransaction().set_sensitive(false);
               m_headerBar.getBtnDeleteTransaction().set_sensitive(false);
           }
        });
        //ScrollWindow
        m_scrollDataTransactions.set_child(m_dataTransactions);
        m_scrollDataTransactions.set_margin(6);
        m_scrollDataTransactions.set_expand(true);
        //==Shortcuts==//
        m_shortcutController = Gtk::ShortcutController::create();
        m_shortcutController->set_scope(Gtk::ShortcutScope::GLOBAL);
        //New Account
        m_shortcutNewAccountTrigger = Gtk::KeyvalTrigger::create(GDK_KEY_N, Gdk::ModifierType::CONTROL_MASK);
        m_shortcutNewAccountAction = Gtk::CallbackAction::create([&](Gtk::Widget& widget, const Glib::VariantBase& args)
        {
            newAccount(args);
            return true;
        });
        m_shortcutNewAccount = Gtk::Shortcut::create(m_shortcutNewAccountTrigger, m_shortcutNewAccountAction);
        m_shortcutController->add_shortcut(m_shortcutNewAccount);
        //Open Account
        m_shortcutOpenAccountTrigger = Gtk::KeyvalTrigger::create(GDK_KEY_O, Gdk::ModifierType::CONTROL_MASK);
        m_shortcutOpenAccountAction = Gtk::CallbackAction::create([&](Gtk::Widget& widget, const Glib::VariantBase& args)
        {
            openAccount(args);
            return true;
        });
        m_shortcutOpenAccount = Gtk::Shortcut::create(m_shortcutOpenAccountTrigger, m_shortcutOpenAccountAction);
        m_shortcutController->add_shortcut(m_shortcutOpenAccount);
        //Close Account
        m_shortcutCloseAccountTrigger = Gtk::KeyvalTrigger::create(GDK_KEY_W, Gdk::ModifierType::CONTROL_MASK);
        m_shortcutCloseAccountAction = Gtk::CallbackAction::create([&](Gtk::Widget& widget, const Glib::VariantBase& args)
        {
            if(m_account.has_value())
            {
                closeAccount(args);
            }
            return true;
        });
        m_shortcutCloseAccount = Gtk::Shortcut::create(m_shortcutCloseAccountTrigger, m_shortcutCloseAccountAction);
        m_shortcutController->add_shortcut(m_shortcutCloseAccount);
        //Backup Account
        m_shortcutBackupAccountTrigger = Gtk::KeyvalTrigger::create(GDK_KEY_B, Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::SHIFT_MASK);
        m_shortcutBackupAccountAction = Gtk::CallbackAction::create([&](Gtk::Widget& widget, const Glib::VariantBase& args)
        {
            if(m_account.has_value())
            {
                backupAccount();
            }
            return true;
        });
        m_shortcutBackupAccount = Gtk::Shortcut::create(m_shortcutBackupAccountTrigger, m_shortcutBackupAccountAction);
        m_shortcutController->add_shortcut(m_shortcutBackupAccount);
        //Restore Account
        m_shortcutRestoreAccountTrigger = Gtk::KeyvalTrigger::create(GDK_KEY_R, Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::SHIFT_MASK);
        m_shortcutRestoreAccountAction = Gtk::CallbackAction::create([&](Gtk::Widget& widget, const Glib::VariantBase& args)
        {
            if(m_account.has_value())
            {
                restoreAccount();
            }
            return true;
        });
        m_shortcutRestoreAccount = Gtk::Shortcut::create(m_shortcutRestoreAccountTrigger, m_shortcutRestoreAccountAction);
        m_shortcutController->add_shortcut(m_shortcutRestoreAccount);
        //New Transaction
        m_shortcutNewTransactionTrigger = Gtk::KeyvalTrigger::create(GDK_KEY_N, Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::SHIFT_MASK);
        m_shortcutNewTransactionAction = Gtk::CallbackAction::create([&](Gtk::Widget& widget, const Glib::VariantBase& args)
        {
            if(m_account.has_value())
            {
                newTransaction();
            }
            return true;
        });
        m_shortcutNewTransaction = Gtk::Shortcut::create(m_shortcutNewTransactionTrigger, m_shortcutNewTransactionAction);
        m_shortcutController->add_shortcut(m_shortcutNewTransaction);
        //Edit Transaction
        m_shortcutEditTransactionTrigger = Gtk::KeyvalTrigger::create(GDK_KEY_O, Gdk::ModifierType::CONTROL_MASK | Gdk::ModifierType::SHIFT_MASK);
        m_shortcutEditTransactionAction = Gtk::CallbackAction::create([&](Gtk::Widget& widget, const Glib::VariantBase& args)
        {
            if(m_dataTransactions.get_selection()->get_selected_rows().size() == 1)
            {
                editTransaction();
            }
            return true;
        });
        m_shortcutEditTransaction = Gtk::Shortcut::create(m_shortcutEditTransactionTrigger, m_shortcutEditTransactionAction);
        m_shortcutController->add_shortcut(m_shortcutEditTransaction);
        //Delete Transaction
        m_shortcutDeleteTransactionTrigger = Gtk::KeyvalTrigger::create(GDK_KEY_Delete);
        m_shortcutDeleteTransactionAction = Gtk::CallbackAction::create([&](Gtk::Widget& widget, const Glib::VariantBase& args)
        {
            if(m_dataTransactions.get_selection()->get_selected_rows().size() == 1)
            {
                m_headerBar.getPopDeleteTransaction().popup();
            }
            return true;
        });
        m_shortcutDeleteTransaction = Gtk::Shortcut::create(m_shortcutDeleteTransactionTrigger, m_shortcutDeleteTransactionAction);
        m_shortcutController->add_shortcut(m_shortcutDeleteTransaction);
        //About
        m_shortcutAboutTrigger = Gtk::KeyvalTrigger::create(GDK_KEY_F1);
        m_shortcutAboutAction = Gtk::CallbackAction::create([&](Gtk::Widget& widget, const Glib::VariantBase& args)
        {
            about(args);
            return true;
        });
        m_shortcutAbout = Gtk::Shortcut::create(m_shortcutAboutTrigger, m_shortcutAboutAction);
        m_shortcutController->add_shortcut(m_shortcutAbout);
        add_controller(m_shortcutController);
        //==Layout==//
        m_mainBox.set_orientation(Gtk::Orientation::VERTICAL);
        m_mainBox.append(m_infoBar);
        m_mainBox.append(m_gridAccountInfo);
        m_mainBox.append(m_scrollDataTransactions);
        set_child(m_mainBox);
        maximize();
    }

    MainWindow::~MainWindow()
    {
        //==Save Config==//
        Configuration configuration;
        if(configuration.rememberLastOpenedAccount())
        {
            configuration.setLastOpenedAccount(m_account.has_value() ? m_account->getPath() : "");
        }
        configuration.save();
    }

    void MainWindow::onShow()
    {
        if(!m_opened)
        {
            m_opened = true;
            //==Load Config==//
            Configuration configuration;
            if(configuration.rememberLastOpenedAccount() && std::filesystem::exists(configuration.getLastOpenedAccount()))
            {
                m_account = std::make_optional<Account>(configuration.getLastOpenedAccount());
                m_headerBar.setSubtitle(m_account->getPath());
                m_headerBar.getActionCloseAccount()->set_enabled(true);
                m_headerBar.getBtnNewTransaction().set_sensitive(true);
                m_headerBar.getBtnBackupAccount().set_sensitive(true);
                m_headerBar.getBtnRestoreAccount().set_sensitive(true);
                reloadAccount();
            }
        }
    }

    void MainWindow::newAccount(const Glib::VariantBase& args)
    {
        Gtk::FileChooserDialog* folderDialog = new Gtk::FileChooserDialog(*this, "Save New Account File", Gtk::FileChooserDialog::Action::SAVE, true);
        folderDialog->set_modal(true);
        Gtk::Button* btnSave = folderDialog->add_button("_Save", Gtk::ResponseType::OK);
        btnSave->get_style_context()->add_class("suggested-action");
        folderDialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
        std::shared_ptr<Gtk::FileFilter> accountFileFilter = Gtk::FileFilter::create();
        accountFileFilter->set_name("Nickvision Money Account");
        accountFileFilter->add_pattern("*.nmoney");
        folderDialog->add_filter(accountFileFilter);
        folderDialog->signal_response().connect(sigc::bind([&](int response, Gtk::FileChooserDialog* dialog)
        {
            if(response == Gtk::ResponseType::OK)
            {
                std::string newPath = dialog->get_file()->get_path();
                if(std::filesystem::path(newPath).extension().empty())
                {
                    newPath += ".nmoney";
                }
                delete dialog;
                m_account = std::make_optional<Account>(newPath);
                m_headerBar.setSubtitle(m_account->getPath());
                m_headerBar.getActionCloseAccount()->set_enabled(true);
                m_headerBar.getBtnNewTransaction().set_sensitive(true);
                m_headerBar.getBtnBackupAccount().set_sensitive(true);
                m_headerBar.getBtnRestoreAccount().set_sensitive(true);
                reloadAccount();
            }
            else
            {
                delete dialog;
            }
        }, folderDialog));
        folderDialog->show();
    }

    void MainWindow::openAccount(const Glib::VariantBase& args)
    {
        Gtk::FileChooserDialog* folderDialog = new Gtk::FileChooserDialog(*this, "Open Account File", Gtk::FileChooserDialog::Action::OPEN, true);
        folderDialog->set_modal(true);
        Gtk::Button* btnOpen = folderDialog->add_button("_Open", Gtk::ResponseType::OK);
        btnOpen->get_style_context()->add_class("suggested-action");
        folderDialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
        std::shared_ptr<Gtk::FileFilter> accountFileFilter = Gtk::FileFilter::create();
        accountFileFilter->set_name("Nickvision Money Account");
        accountFileFilter->add_pattern("*.nmoney");
        folderDialog->add_filter(accountFileFilter);
        folderDialog->signal_response().connect(sigc::bind([&](int response, Gtk::FileChooserDialog* dialog)
        {
            if(response == Gtk::ResponseType::OK)
            {
                std::string newPath = dialog->get_file()->get_path();
                delete dialog;
                m_account = std::make_optional<Account>(newPath);
                m_headerBar.setSubtitle(m_account->getPath());
                m_headerBar.getActionCloseAccount()->set_enabled(true);
                m_headerBar.getBtnNewTransaction().set_sensitive(true);
                m_headerBar.getBtnBackupAccount().set_sensitive(true);
                m_headerBar.getBtnRestoreAccount().set_sensitive(true);
                reloadAccount();
            }
            else
            {
                delete dialog;
            }
        }, folderDialog));
        folderDialog->show();
    }

    void MainWindow::closeAccount(const Glib::VariantBase& args)
    {
        m_account = std::nullopt;
        m_headerBar.setSubtitle("No Account Opened");
        m_headerBar.getActionCloseAccount()->set_enabled(false);
        m_headerBar.getBtnNewTransaction().set_sensitive(false);
        m_headerBar.getBtnBackupAccount().set_sensitive(false);
        m_headerBar.getBtnRestoreAccount().set_sensitive(false);
        reloadAccount();
    }

    void MainWindow::newTransaction()
    {
        TransactionDialog* transactionDialog = new TransactionDialog(*this, *m_account);
        transactionDialog->signal_response().connect(sigc::bind([&](int response, TransactionDialog* dialog)
        {
            if(response != Gtk::ResponseType::NONE)
            {
                delete dialog;
                if(response == Gtk::ResponseType::OK)
                {
                    reloadAccount();
                }
            }
        }, transactionDialog));
        transactionDialog->show();
    }

    void MainWindow::editTransaction()
    {
        TransactionDialog* transactionDialog = new TransactionDialog(*this, *m_account, m_dataTransactions.get_selection()->get_selected()->get_value(m_dataTransactionsColumns.getColID()));
        transactionDialog->signal_response().connect(sigc::bind([&](int response, TransactionDialog* dialog)
        {
            if(response != Gtk::ResponseType::NONE)
            {
                delete dialog;
                if(response == Gtk::ResponseType::OK)
                {
                    reloadAccount();
                }
            }
        }, transactionDialog));
        transactionDialog->show();
    }

    void MainWindow::deleteTransaction()
    {
        m_headerBar.getPopDeleteTransaction().popdown();
        m_account->deleteTransaction(m_dataTransactions.get_selection()->get_selected()->get_value(m_dataTransactionsColumns.getColID()));
        reloadAccount();
    }

    void MainWindow::backupAccount()
    {
        Gtk::FileChooserDialog* folderDialog = new Gtk::FileChooserDialog(*this, "Save Backup Account File", Gtk::FileChooserDialog::Action::SAVE, true);
        folderDialog->set_modal(true);
        Gtk::Button* btnSave = folderDialog->add_button("_Save", Gtk::ResponseType::OK);
        btnSave->get_style_context()->add_class("suggested-action");
        folderDialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
        std::shared_ptr<Gtk::FileFilter> accountFileFilter = Gtk::FileFilter::create();
        accountFileFilter->set_name("Nickvision Money Backup");
        accountFileFilter->add_pattern("*.nmoneyb");
        folderDialog->add_filter(accountFileFilter);
        folderDialog->signal_response().connect(sigc::bind([&](int response, Gtk::FileChooserDialog* dialog)
        {
            if(response == Gtk::ResponseType::OK)
            {
                std::string backupPath = dialog->get_file()->get_path();
                if(std::filesystem::path(backupPath).extension().empty())
                {
                    backupPath += ".nmoneyb";
                }
                delete dialog;
                m_account->backup(backupPath);
                m_infoBar.showMessage("Backup Successful", "Saved to: " + backupPath);
            }
            else
            {
                delete dialog;
            }
        }, folderDialog));
        folderDialog->show();
    }

    void MainWindow::restoreAccount()
    {
        Gtk::FileChooserDialog* folderDialog = new Gtk::FileChooserDialog(*this, "Open Backup File", Gtk::FileChooserDialog::Action::OPEN, true);
        folderDialog->set_modal(true);
        Gtk::Button* btnOpen = folderDialog->add_button("_Open", Gtk::ResponseType::OK);
        btnOpen->get_style_context()->add_class("suggested-action");
        folderDialog->add_button("_Cancel", Gtk::ResponseType::CANCEL);
        std::shared_ptr<Gtk::FileFilter> accountFileFilter = Gtk::FileFilter::create();
        accountFileFilter->set_name("Nickvision Money Backup");
        accountFileFilter->add_pattern("*.nmoneyb");
        folderDialog->add_filter(accountFileFilter);
        folderDialog->signal_response().connect(sigc::bind([&](int response, Gtk::FileChooserDialog* dialog)
        {
            if(response == Gtk::ResponseType::OK)
            {
                std::string restorePath = dialog->get_file()->get_path();
                delete dialog;
                m_account->restore(restorePath);
                m_infoBar.showMessage("Restore Successful", "Restored from: " + restorePath);
                reloadAccount();
            }
            else
            {
                delete dialog;
            }
        }, folderDialog));
        folderDialog->show();
    }

    void MainWindow::checkForUpdates(const Glib::VariantBase& args)
    {
        ProgressDialog* checkingDialog = new ProgressDialog(*this, "Checking for updates...", [&]() { m_updater.checkForUpdates(); });
        checkingDialog->signal_hide().connect(sigc::bind([&](ProgressDialog* dialog)
        {
            delete dialog;
            if(m_updater.updateAvailable())
            {
                Gtk::MessageDialog* updateDialog = new Gtk::MessageDialog(*this, "Update Available", false, Gtk::MessageType::INFO, Gtk::ButtonsType::YES_NO, true);
                updateDialog->set_secondary_text("\n===V" + m_updater.getLatestVersion()->toString() + " Changelog===\n" + m_updater.getChangelog() + "\n\nNickvision Money can automatically download the latest executable to your Downloads directory. Would you like to continue?");
                updateDialog->signal_response().connect(sigc::bind([&](int response, Gtk::MessageDialog* dialog)
                {
                    delete dialog;
                    if(response == Gtk::ResponseType::YES)
                    {
                        bool* success = new bool(false);
                        ProgressDialog* downloadingDialog = new ProgressDialog(*this, "Downloading the update...", [&]() { *success = m_updater.update(); });
                        downloadingDialog->signal_hide().connect(sigc::bind([&](ProgressDialog* dialog, bool* success)
                        {
                            delete dialog;
                            if(*success)
                            {
                                m_infoBar.showMessage("Download Successful", "We recommend moving the new version out of your Downloads directory and running it from elsewhere to allow future updates to download smoothly.");
                            }
                            else
                            {
                                m_infoBar.showMessage("Error", "Unable to download the executable. Please try again. If the issue continues, file a bug report.");
                            }
                            delete success;
                        }, downloadingDialog, success));
                        downloadingDialog->show();
                    }
                }, updateDialog));
                updateDialog->show();
            }
            else
            {
                m_infoBar.showMessage("No Update Available", "There is no update at this time. Please check again later.");
            }
        }, checkingDialog));
        checkingDialog->show();
    }

    void MainWindow::gitHubRepo(const Glib::VariantBase& args)
    {
        Gio::AppInfo::launch_default_for_uri("https://github.com/nlogozzo/NickvisionMoney");
    }

    void MainWindow::reportABug(const Glib::VariantBase& args)
    {
        Gio::AppInfo::launch_default_for_uri("https://github.com/nlogozzo/NickvisionMoney/issues/new");
    }

    void MainWindow::settings(const Glib::VariantBase& args)
    {
        SettingsDialog* settingsDialog = new SettingsDialog(*this);
        settingsDialog->signal_hide().connect(sigc::bind([](SettingsDialog* dialog)
        {
            delete dialog;
        }, settingsDialog));
        settingsDialog->show();
    }

    void MainWindow::shortcuts(const Glib::VariantBase& args)
    {
        ShortcutsWindow* shortcutsWindow = new ShortcutsWindow(*this);
        shortcutsWindow->signal_hide().connect(sigc::bind([](ShortcutsWindow* window)
        {
            delete window;
        }, shortcutsWindow));
        shortcutsWindow->show();
    }

    void MainWindow::changelog(const Glib::VariantBase& args)
    {
        Gtk::MessageDialog* changelogDialog = new Gtk::MessageDialog(*this, "What's New?", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK, true);
        changelogDialog->set_secondary_text("\n- Added keyboard shortcuts");
        changelogDialog->signal_response().connect(sigc::bind([](int response, Gtk::MessageDialog* dialog)
        {
           delete dialog;
        }, changelogDialog));
        changelogDialog->show();
    }

    void MainWindow::about(const Glib::VariantBase& args)
    {
        Gtk::AboutDialog* aboutDialog = new Gtk::AboutDialog();
        aboutDialog->set_transient_for(*this);
        aboutDialog->set_modal(true);
        aboutDialog->set_hide_on_close(true);
        aboutDialog->set_program_name("Nickvision Money");
        aboutDialog->set_version("2022.1.3");
        aboutDialog->set_comments("A personal finance manager.");
        aboutDialog->set_copyright("(C) Nickvision 2021-2022");
        aboutDialog->set_license_type(Gtk::License::GPL_3_0);
        aboutDialog->set_website("https://github.com/nlogozzo");
        aboutDialog->set_website_label("GitHub");
        aboutDialog->set_authors({ "Nicholas Logozzo" });
        aboutDialog->signal_hide().connect(sigc::bind([](Gtk::AboutDialog* dialog)
        {
           delete dialog;
        }, aboutDialog));
        aboutDialog->show();
    }

    void MainWindow::onRowDoubleClick(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
    {
        editTransaction();
    }

    void MainWindow::reloadAccount()
    {
        m_dataTransactionsModel->clear();
        if(m_account.has_value())
        {
            m_txtIncome.set_text(m_account->getIncomeAsString());
            m_txtExpense.set_text(m_account->getExpenseAsString());
            m_txtTotal.set_text(m_account->getTotalAsString());
            for(const std::pair<unsigned int, Transaction>& pair : m_account->getTransactions())
            {
                Gtk::TreeRow row = *(m_dataTransactionsModel->append());
                row[m_dataTransactionsColumns.getColID()] = pair.second.getID();
                row[m_dataTransactionsColumns.getColDate()] = pair.second.getDate().substr(0, 10);
                row[m_dataTransactionsColumns.getColDescription()] = pair.second.getDescription();
                row[m_dataTransactionsColumns.getColType()] = pair.second.getTypeAsString();
                row[m_dataTransactionsColumns.getColRepeatInterval()] = pair.second.getRepeatIntervalAsString();
                row[m_dataTransactionsColumns.getColAmount()] = pair.second.getAmountAsString();
            }
        }
        else
        {
            m_txtIncome.set_text("");
            m_txtExpense.set_text("");
            m_txtTotal.set_text("");
        }
        m_dataTransactions.columns_autosize();
    }
}
