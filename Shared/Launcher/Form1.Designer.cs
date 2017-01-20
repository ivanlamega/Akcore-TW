namespace AKCore_Launcher
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.Picturebox = new System.Windows.Forms.PictureBox();
            this.btnDatabase = new System.Windows.Forms.Button();
            this.btnAuth = new System.Windows.Forms.Button();
            this.btnChar = new System.Windows.Forms.Button();
            this.btnComm = new System.Windows.Forms.Button();
            this.btnGame = new System.Windows.Forms.Button();
            this.lblWeb = new System.Windows.Forms.Label();
            this.lblAuth = new System.Windows.Forms.Label();
            this.lblChar = new System.Windows.Forms.Label();
            this.lblComm = new System.Windows.Forms.Label();
            this.lblWorld = new System.Windows.Forms.Label();
            this.btnConsole = new System.Windows.Forms.Button();
            this.lblConsole = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.Picturebox)).BeginInit();
            this.SuspendLayout();
            // 
            // Picturebox
            // 
            this.Picturebox.ErrorImage = global::AKCore_Launcher.Properties.Resources.akcore;
            this.Picturebox.Image = global::AKCore_Launcher.Properties.Resources.akcore;
            this.Picturebox.InitialImage = global::AKCore_Launcher.Properties.Resources.akcore;
            this.Picturebox.Location = new System.Drawing.Point(2, -2);
            this.Picturebox.Name = "Picturebox";
            this.Picturebox.Size = new System.Drawing.Size(369, 100);
            this.Picturebox.TabIndex = 0;
            this.Picturebox.TabStop = false;
            // 
            // btnDatabase
            // 
            this.btnDatabase.Location = new System.Drawing.Point(12, 139);
            this.btnDatabase.Name = "btnDatabase";
            this.btnDatabase.Size = new System.Drawing.Size(241, 54);
            this.btnDatabase.TabIndex = 1;
            this.btnDatabase.Text = "Webserver And Database";
            this.btnDatabase.UseVisualStyleBackColor = true;
            this.btnDatabase.Click += new System.EventHandler(this.btnDatabase_Click);
            // 
            // btnAuth
            // 
            this.btnAuth.Location = new System.Drawing.Point(12, 199);
            this.btnAuth.Name = "btnAuth";
            this.btnAuth.Size = new System.Drawing.Size(241, 54);
            this.btnAuth.TabIndex = 2;
            this.btnAuth.Text = "Authorization Server";
            this.btnAuth.UseVisualStyleBackColor = true;
            this.btnAuth.Click += new System.EventHandler(this.btnAuth_Click);
            // 
            // btnChar
            // 
            this.btnChar.Location = new System.Drawing.Point(12, 259);
            this.btnChar.Name = "btnChar";
            this.btnChar.Size = new System.Drawing.Size(241, 54);
            this.btnChar.TabIndex = 3;
            this.btnChar.Text = "Character Server";
            this.btnChar.UseVisualStyleBackColor = true;
            this.btnChar.Click += new System.EventHandler(this.btnChar_Click);
            // 
            // btnComm
            // 
            this.btnComm.Location = new System.Drawing.Point(12, 319);
            this.btnComm.Name = "btnComm";
            this.btnComm.Size = new System.Drawing.Size(241, 54);
            this.btnComm.TabIndex = 4;
            this.btnComm.Text = "Community Server";
            this.btnComm.UseVisualStyleBackColor = true;
            this.btnComm.Click += new System.EventHandler(this.btnComm_Click);
            // 
            // btnGame
            // 
            this.btnGame.Location = new System.Drawing.Point(12, 379);
            this.btnGame.Name = "btnGame";
            this.btnGame.Size = new System.Drawing.Size(241, 54);
            this.btnGame.TabIndex = 5;
            this.btnGame.Text = "World Server";
            this.btnGame.UseVisualStyleBackColor = true;
            this.btnGame.Click += new System.EventHandler(this.btnGame_Click);
            // 
            // lblWeb
            // 
            this.lblWeb.AutoSize = true;
            this.lblWeb.Location = new System.Drawing.Point(295, 160);
            this.lblWeb.Name = "lblWeb";
            this.lblWeb.Size = new System.Drawing.Size(67, 13);
            this.lblWeb.TabIndex = 6;
            this.lblWeb.Text = "Not Running";
            // 
            // lblAuth
            // 
            this.lblAuth.AutoSize = true;
            this.lblAuth.Location = new System.Drawing.Point(295, 220);
            this.lblAuth.Name = "lblAuth";
            this.lblAuth.Size = new System.Drawing.Size(67, 13);
            this.lblAuth.TabIndex = 7;
            this.lblAuth.Text = "Not Running";
            // 
            // lblChar
            // 
            this.lblChar.AutoSize = true;
            this.lblChar.Location = new System.Drawing.Point(295, 280);
            this.lblChar.Name = "lblChar";
            this.lblChar.Size = new System.Drawing.Size(67, 13);
            this.lblChar.TabIndex = 8;
            this.lblChar.Text = "Not Running";
            // 
            // lblComm
            // 
            this.lblComm.AutoSize = true;
            this.lblComm.Location = new System.Drawing.Point(295, 340);
            this.lblComm.Name = "lblComm";
            this.lblComm.Size = new System.Drawing.Size(67, 13);
            this.lblComm.TabIndex = 9;
            this.lblComm.Text = "Not Running";
            // 
            // lblWorld
            // 
            this.lblWorld.AutoSize = true;
            this.lblWorld.Location = new System.Drawing.Point(295, 400);
            this.lblWorld.Name = "lblWorld";
            this.lblWorld.Size = new System.Drawing.Size(67, 13);
            this.lblWorld.TabIndex = 10;
            this.lblWorld.Text = "Not Running";
            // 
            // btnConsole
            // 
            this.btnConsole.Location = new System.Drawing.Point(12, 104);
            this.btnConsole.Name = "btnConsole";
            this.btnConsole.Size = new System.Drawing.Size(241, 29);
            this.btnConsole.TabIndex = 11;
            this.btnConsole.Text = "Toggle Consoles";
            this.btnConsole.UseVisualStyleBackColor = true;
            this.btnConsole.Click += new System.EventHandler(this.btnConsole_Click);
            // 
            // lblConsole
            // 
            this.lblConsole.AutoSize = true;
            this.lblConsole.Location = new System.Drawing.Point(295, 112);
            this.lblConsole.Name = "lblConsole";
            this.lblConsole.Size = new System.Drawing.Size(41, 13);
            this.lblConsole.TabIndex = 12;
            this.lblConsole.Text = "Hidden";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(374, 436);
            this.Controls.Add(this.lblConsole);
            this.Controls.Add(this.btnConsole);
            this.Controls.Add(this.lblWorld);
            this.Controls.Add(this.lblComm);
            this.Controls.Add(this.lblChar);
            this.Controls.Add(this.lblAuth);
            this.Controls.Add(this.lblWeb);
            this.Controls.Add(this.btnGame);
            this.Controls.Add(this.btnComm);
            this.Controls.Add(this.btnChar);
            this.Controls.Add(this.btnAuth);
            this.Controls.Add(this.btnDatabase);
            this.Controls.Add(this.Picturebox);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "AKCore Launcher";
            ((System.ComponentModel.ISupportInitialize)(this.Picturebox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox Picturebox;
        private System.Windows.Forms.Button btnDatabase;
        private System.Windows.Forms.Button btnAuth;
        private System.Windows.Forms.Button btnChar;
        private System.Windows.Forms.Button btnComm;
        private System.Windows.Forms.Button btnGame;
        private System.Windows.Forms.Label lblWeb;
        private System.Windows.Forms.Label lblAuth;
        private System.Windows.Forms.Label lblChar;
        private System.Windows.Forms.Label lblComm;
        private System.Windows.Forms.Label lblWorld;
        private System.Windows.Forms.Button btnConsole;
        private System.Windows.Forms.Label lblConsole;
    }
}

