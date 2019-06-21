//
//  about.swift
//  Shrine
//
//  Created by Zaid Aggad on 06/06/2019.
//  Copyright © 2019 Google. All rights reserved.
//

import UIKit
import MaterialComponents
import MessageUI

class about: UIViewController, MFMailComposeViewControllerDelegate {
    
    func mailComposeController(_ controller: MFMailComposeViewController, didFinishWith result: MFMailComposeResult, error: Error?) {
        self.dismiss(animated: true)
    }
    var appBarViewController = MDCAppBarViewController()
    override func viewDidLoad() {
        super.viewDidLoad()
        self.view.backgroundColor = .white
        self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.headerView.backgroundColor = GMColor.imperialblue()
        self.appBarViewController.navigationController?.navigationBar.backgroundColor = GMColor.imperialblue()
        self.view.tintColor = .white
        let titleDict: NSDictionary = [NSAttributedStringKey.foregroundColor: UIColor.white]
        self.appBarViewController.navigationBar.titleTextAttributes = (titleDict as! [NSAttributedStringKey : Any])
        self.view.tintColor = .white
        self.title = "About"
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
    @objc func menuItemTapped(sender: Any) {
        let transition = CATransition()
        transition.duration = 0.3
        transition.type = kCATransitionPush
        transition.subtype = kCATransitionFromLeft
        transition.timingFunction = CAMediaTimingFunction(name: kCAMediaTimingFunctionEaseInEaseOut)
        view.window!.layer.add(transition, forKey: kCATransition)
        self.dismiss(animated: false, completion: nil)
    }

    @IBAction func emailB(_ sender: Any) {
        
        sendEmail()

        
    }
    
    @IBAction func git(_ sender: Any) {
        UIApplication.shared.openURL(NSURL(string: "https://github.com/ledangaravi/remote-patient-monitoring")! as URL)
    }
    func sendEmail() {
        if MFMailComposeViewController.canSendMail() {
            let mail = MFMailComposeViewController()
            mail.mailComposeDelegate = self
            mail.setToRecipients(["zaid.aggad98@gmail.com"])
            mail.setMessageBody("<p>Dear P24 team,</p>", isHTML: true)
            
            present(mail, animated: true)
        } else {
            // show failure alert
        }
    }
 

    

    /*
     @IBAction func emailB(_ sender: Any) {
     }
     // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destination.
        // Pass the selected object to the new view controller.
    }
    */

}
