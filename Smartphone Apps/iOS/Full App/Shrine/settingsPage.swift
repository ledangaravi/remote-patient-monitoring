//
//  settingsPage.swift
//  Shrine
//
//  Created by Zaid Aggad on 02/06/2019.
//  Copyright © 2019 Google. All rights reserved.
//

import UIKit
import MaterialComponents
import MessageUI
let Phonekey = "Phone Number"

class settingsPage: UIViewController, MFMessageComposeViewControllerDelegate,  UITextFieldDelegate  {
    
    func textFieldShouldReturn(_ textField: UITextField) -> Bool {
        self.view.endEditing(true)
        return false
    }
    
    func messageComposeViewController(_ controller: MFMessageComposeViewController, didFinishWith result: MessageComposeResult) {
        print("I am trying to cancel!")
        // Dismiss the message compose view controller.
        controller.dismiss(animated: true, completion: nil)
    }
    

    @IBOutlet weak var phoneNum: UITextField!
    @IBOutlet weak var saveB: UIButton!
    
    var appBarViewController = MDCAppBarViewController()
    override func viewDidLoad() {
        super.viewDidLoad()
        self.phoneNum.delegate = self
        self.checklable()
        self.view.backgroundColor = .white
        self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.headerView.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
        self.view.tintColor = .white
        let titleDict: NSDictionary = [NSAttributedStringKey.foregroundColor: UIColor.white]
        self.appBarViewController.navigationBar.titleTextAttributes = (titleDict as! [NSAttributedStringKey : Any])
        self.view.tintColor = .white
        self.title = "Settings"
        self.addChildViewController(self.appBarViewController)
        self.view.addSubview(self.appBarViewController.view)
        self.appBarViewController.didMove(toParentViewController: self)
        
        let menuItemImage = UIImage(named: "MenuItem")
        let templatedMenuItemImage = menuItemImage?.withRenderingMode(.alwaysTemplate)
        let menuItem = UIBarButtonItem(image: templatedMenuItemImage,
                                       style: .plain,
                                       target: self,
                                       action: #selector(menuItemTapped(sender:)))
        self.navigationItem.leftBarButtonItem = menuItem
        
    }
    override func viewDidAppear(_ animated: Bool) {
        if let x = UserDefaults.standard.object(forKey: Phonekey) as? String{
            phoneNum.text = x
        }
        
    }
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }                       
     @objc func menuItemTapped(sender: Any) {
        let transition = CATransition()
        transition.duration = 0.3
        transition.type = kCATransitionPush
        transition.subtype = kCATransitionFromLeft
        transition.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
        view.window!.layer.add(transition, forKey: kCATransition)
        self.dismiss(animated: false, completion: nil)
    }
    

    @IBAction func setNum(_ sender: Any) {
        UserDefaults.standard.set(phoneNum.text, forKey: Phonekey)
        if phoneNum.text != nil{
        phoneNum.text = phoneNum.text!
        saveB.titleLabel?.text = "Change"
        }
    }
    func checklable(){
        if UserDefaults.standard.object(forKey: Phonekey) != nil{
            saveB.setTitle("Change", for: .normal)
        }
    }
    
    @IBAction func sosprotocal(_ sender: Any) {
        displayMessageInterface()
    }
    func displayMessageInterface() {
        let composeVC = MFMessageComposeViewController()
        composeVC.messageComposeDelegate = self
        
        // Configure the fields of the interface.
        let x = UserDefaults.standard.object(forKey: Phonekey) as? String
        composeVC.recipients = [x!]
        composeVC.body = "I Need Help!"
        
        // Present the view controller modally.
        if MFMessageComposeViewController.canSendText() {
            self.present(composeVC, animated: true, completion: nil)
        } else {
            print("Can't send messages.")
        }
    }
    
    
    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destination.
        // Pass the selected object to the new view controller.
    }
    */

}
